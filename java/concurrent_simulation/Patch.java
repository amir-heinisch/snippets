package com.concurrent_simulation.simulation.rocket;

import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import com.concurrent_simulation.model.PersonInfo;
import com.concurrent_simulation.model.Query;
import com.concurrent_simulation.model.Rectangle;
import com.concurrent_simulation.model.Scenario;
import com.concurrent_simulation.model.Statistics;
import com.concurrent_simulation.model.XY;
import com.concurrent_simulation.simulation.common.Context;
import com.concurrent_simulation.simulation.common.Utils;
import com.concurrent_simulation.simulation.common.Person;
import com.concurrent_simulation.validator.Validator;


public class Patch implements Runnable, Context {

    private final int id;
    private final int k;
    private final int padding;
    private final Scenario scenario;
    private final Validator validator;

    // Variables needed for context interface.
    private final Rectangle grid;
    private List<Person> population;
    // Holds historic main population.
    private final Map<Integer,List<Person>> histPopulation;

    // Holds information for the main grid + padding.
    private Rectangle extGrid;
    private final List<Rectangle> extObstacles;
    private List<Person> extPopulation;

    // Need to call some methods on those objects.
    private final List<Patch> neighbours;
    private List<Patch> tmpNeighbours;

    // Holds the persons for each round to output.
    private final List<Map<Integer,PersonInfo>> populationTrace;
    // Holds some statistics to output.
    private final Map<String,List<Statistics>> statistics;

    // Concurrency helpers.
    private final ReadWriteLock histPopulationLock;
    private final Condition histKeyAvailable;

    public Patch(int id, int k, int padding, Rectangle grid, Scenario scenario, Validator validator) {
        // Save some variables.
        this.id = id;
        this.k = k;
        this.grid = grid;
        this.extGrid = null;
        this.scenario = scenario;
        this.validator = validator;
        this.padding = padding;
        // Init some variables.
        this.population = new LinkedList<Person>();
        this.histPopulation = new HashMap<Integer,List<Person>>();
        this.extPopulation = new LinkedList<Person>();
        this.extObstacles = new LinkedList<Rectangle>();
        this.populationTrace = new LinkedList<Map<Integer,PersonInfo>>();
        this.statistics = new HashMap<String, List<Statistics>>();
        this.neighbours = new LinkedList<Patch>();
        this.tmpNeighbours = new LinkedList<Patch>();
        this.histPopulationLock = new ReentrantReadWriteLock();
        this.histKeyAvailable = this.histPopulationLock.writeLock().newCondition();
    }

    // Interface: Runnable.

    @Override
    public void run() {
        // Setup stats.
        for (String queryKey : scenario.getQueries().keySet()) {
            this.statistics.put(queryKey, new LinkedList<>());
        }

        // Calculate extended grid.
        this.extGrid = this.calcExtGrid(grid, this.padding, scenario.getGridSize());

        // Place obstacles on extended patch.
        for (Rectangle obstacle : this.scenario.getObstacles()) {
            if (obstacle.overlaps(this.extGrid))
                this.extObstacles.add(obstacle.intersect(this.extGrid));
        }

        // Filter neighbours.
        for (Patch neighbour : this.tmpNeighbours) {
            // Check for overlap and no obstacles in between.
            if (neighbour != this &&
                this.extGrid.overlaps(neighbour.getMainGrid()) &&
                Utils.mayPropagateFrom(this.scenario, neighbour.getMainGrid(), this.grid))
                    this.neighbours.add(neighbour);
        }

        // Save output for initial round.
        // NOTE! Can't get placed in constructor.
        this.addPopulationTraceEntry();
        this.addStatisticsEntry();

        // Run simulation.
        for (int tick = 0; tick < this.scenario.getTicks(); tick++) {

            // Sync if needed.
            if ((tick % this.k) == 0) {
                // Save historic core population.
                List<Person> listClone = new LinkedList<Person>();
                for (Person person : this.population) {
                    listClone.add(person.clone(this));
                }
                this.histPopulationLock.writeLock().lock();
                try {
                    this.histPopulation.put(tick, listClone);
                    // Remove old history.
                    if (this.histPopulation.size() > 2) {
                        this.histPopulation.remove(tick - 2*this.k);
                    }
                    this.histKeyAvailable.signalAll();
                } finally {
                    this.histPopulationLock.writeLock().unlock();
                }
                this.updateExtPatch(tick);
            }

            validator.onPatchTick(tick, this.id);

            // NOTE! extPopulation is only used locally in a non concurrent way.
            for (Person person : this.extPopulation) {
                validator.onPersonTick(tick, this.id, person.getId());
                person.tick();
            }
            // bust the ghosts of all persons
            this.extPopulation.stream().forEach(Person::bustGhost);

            // now compute how the infection spreads between the population
            for (int i = 0; i < this.extPopulation.size(); i++) {
                for (int j = i + 1; j < this.extPopulation.size(); j++) {
                    final Person iPerson = this.extPopulation.get(i);
                    final Person jPerson = this.extPopulation.get(j);
                    final XY iPosition = iPerson.getPosition();
                    final XY jPosition = jPerson.getPosition();
                    final int deltaX = Math.abs(iPosition.getX() - jPosition.getX());
                    final int deltaY = Math.abs(iPosition.getY() - jPosition.getY());
                    final int distance = deltaX + deltaY;
                    if (distance <= this.scenario.getParameters().getInfectionRadius()) {
                        if (iPerson.isInfectious() && iPerson.isCoughing() && jPerson.isBreathing()) {
                            jPerson.infect();
                        }
                        if (jPerson.isInfectious() && jPerson.isCoughing() && iPerson.isBreathing()) {
                            iPerson.infect();
                        }
                    }
                }
            }

            // Rebuild core population.
            this.population = new LinkedList<Person>();
            for (Person person : this.extPopulation) {
                if (this.grid.contains(person.getPosition()))
                    this.addPerson(person);
            }

            // we need to collect statistics and extend the recorded trace
            this.addPopulationTraceEntry();
            this.addStatisticsEntry();
        }
    }

    // Interface: Context

    @Override
    public Rectangle getGrid() {
        return this.extGrid;
    }

    @Override
    public List<Rectangle> getObstacles() {
        return this.extObstacles;
    }

    @Override
    public List<Person> getPopulation() {
        return this.extPopulation;
    }

    // Public methods.

    /**
     * This method returns the main grid (inner) if a thread safe way.
     * NOTE! grid gets only written before concurrency kicks in.
     */
    public Rectangle getMainGrid() {
        return this.grid;
    }

    /**
     * This method returns the population of the main grid
     * in a thread safe way.
     * NOTE! Use getMainPopulation(neighbourId) if you want to sync.
     */
    public List<Person> getMainPopulation(int tick, Patch newPatch) {
        this.histPopulationLock.writeLock().lock();
        try {
            while (!this.histPopulation.containsKey(tick)) {
                this.histKeyAvailable.await();
            }
        } catch (InterruptedException e) {
            // No interrupt called in project -> can't get here.
            e.printStackTrace();
        } finally {
            this.histPopulationLock.writeLock().unlock();
        }

        // Clone list.
        List<Person> listClone = new LinkedList<Person>();
        this.histPopulationLock.readLock().lock();
        try {
            for (Person person : this.histPopulation.get(tick)) {
                if (newPatch.getGrid().contains(person.getPosition()))
                    listClone.add(person.clone(newPatch));
            }
        } finally {
            this.histPopulationLock.readLock().unlock();
        }

        return listClone;
    }

    /**
     * This method returns a populationTrace for a given index.
     * It should only be called after all concurrency finished.
     * NOTE! This method is not thread safe!
     */
    public Map<Integer,PersonInfo> getPopulationTrace(int i) {
        return this.populationTrace.get(i);
    }

    /**
     * This method returns the statistics for this patch.
     * It should only be called after all concurrency finished.
     * NOTE! This method is not thread safe!
     */
    public Map<String,List<Statistics>> getStatistics() {
        return this.statistics;
    }

    /**
     * This method sets a given person on this patch.
     * It should only be called before concurrency kicks in.
     * NOTE! This method is not thread safe!
     */
    public void addPerson(Person person) {
        this.population.add(person);
    }

    /**
     * This method saves only the needed neighbours for the current patch.
     * It should only be called before concurrency kicks in.
     * NOTE! This method is not thread safe!
     */
    public void setNeighbours(List<Patch> neighbours) {
        this.tmpNeighbours = neighbours;
    }

    // Private methods.

    /**
     * This method updates the external patch.
     */
    private void updateExtPatch(int tick) {
        // Get current main population.
        this.extPopulation = new LinkedList<Person>();
        for (Person person : this.population) {
           this.extPopulation.add(person);
        }

        // Shuffle neighbours list.
        Collections.shuffle(this.neighbours);
        // Get current data.
        for (Patch neighbour : this.neighbours) {
            // NOTE! getMainPopulation waits until sync is possible (ticks).
            List<Person> persons = neighbour.getMainPopulation(tick, this);
            for (Person person : persons) {
                this.extPopulation.add(person);
            }
        }

        // Sort population list.
        Collections.sort(this.extPopulation, (p1, p2) -> {
           return p1.getId() - p2.getId();
        });
    }

    /**
     * This method adds a new entry to the statistics in a thread safe way.
     */
    private void addStatisticsEntry() {
        // we collect statistics based on the current SI²R values
        for (Map.Entry<String, Query> entry : this.scenario.getQueries().entrySet()) {
            final Query query = entry.getValue();
            this.statistics.get(entry.getKey()).add(new Statistics(
                this.population.stream().filter(
                    (Person person) ->
                        person.isSusceptible()
                        && query.getArea().contains(person.getPosition())
                ).count(),
                this.population.stream().filter(
                    (Person person) ->
                        person.isInfected()
                        && query.getArea().contains(person.getPosition())
                ).count(),
                this.population.stream().filter(
                    (Person person) ->
                        person.isInfectious()
                        && query.getArea().contains(person.getPosition())
                ).count(),
                this.population.stream().filter(
                    (Person person) ->
                        person.isRecovered()
                        && query.getArea().contains(person.getPosition())
                ).count()
            ));
        }
    }

    /**
     * This method adds a new population trace entry in a thread safe way.
     */
    private void addPopulationTraceEntry() {
        if (this.scenario.getTrace()) {
            Map<Integer,PersonInfo> entry = new HashMap<Integer,PersonInfo>();
            for (Person person : this.population) {
                // NOTE! getInfo already returns personInfo copy.
                entry.put(person.getId(), person.getInfo());
            }
            this.populationTrace.add(entry);
        }
    }

    /**
     * This methods calculates the rectangle of the extended grid
     * (main grid+padding) in a thread safe way.
     */
    private Rectangle calcExtGrid(Rectangle grid, int padding, XY scenarioGrid) {
        // Get dimensions of full simulation board.
        int length = scenarioGrid.getX();
        int height = scenarioGrid.getY();

        // Calculate new top left.
        int newTopX = 0, newTopY = 0;
        XY oldTopLeft = grid.getTopLeft();
        if ((oldTopLeft.getX() - padding) > 0)
            newTopX = oldTopLeft.getX() - padding;
        if ((oldTopLeft.getY() - padding) > 0)
            newTopY = oldTopLeft.getY() - padding;
        XY newTopLeft = new XY(newTopX, newTopY);

        // Calculate new bottom right.
        int newBottomX = (length-1), newBottomY = (height-1);
        XY oldBottomRight = grid.getBottomRight();
        if ((oldBottomRight.getX() + padding) < length)
            newBottomX = oldBottomRight.getX() + padding;
        if ((oldBottomRight.getY() + padding) < height)
            newBottomY = (oldBottomRight.getY() + padding);

        // Calculate new rectangle size.
        int newLength = newBottomX + 1 - newTopX;
        int newHeight = newBottomY + 1 - newTopY;
        XY newSize = new XY(newLength, newHeight);

        // Build new grid.
        return new Rectangle(newTopLeft, newSize);
    }
}
