package com.concurrent_simulation.rocket;

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.concurrent_simulation.model.Output;
import com.concurrent_simulation.model.PersonInfo;
import com.concurrent_simulation.model.Query;
import com.concurrent_simulation.model.Rectangle;
import com.concurrent_simulation.model.Scenario;
import com.concurrent_simulation.model.Statistics;
import com.concurrent_simulation.model.TraceEntry;
import com.concurrent_simulation.simulation.common.Person;
import com.concurrent_simulation.simulation.common.Simulation;
import com.concurrent_simulation.validator.InsufficientPaddingException;
import com.concurrent_simulation.validator.Validator;

/**
 * <p>
 * This class has to implement the <em>Simulation</em> interface.
 * </p>
 */
public class Rocket implements Simulation {

	// Save for getOutput().
	private final Scenario scenario;

	// Holds grid splitted in patches.
	private final List<Patch> patches;

	/**
	 * Constructs a rocket with the given parameters.
	 *
	 * <p>
	 * Throw an insufficient padding exception if and only if the padding is insufficient.
	 * </p>
	 *
	 * @param scenario The scenario to simulate.
	 * @param padding The padding to be used.
	 * @param validator The validator to be called.
	 */
	public Rocket(Scenario scenario, int padding, Validator validator) throws InsufficientPaddingException {
		// Save scenario.
		this.scenario = scenario;

		// Calculate k..throws needed exception.
		int k = this.calculateK(padding);

		// Create patch objects.
		this.patches = new LinkedList<Patch>();
		this.createPatches(k, padding, validator);
		// Give patches neighbour access (references).
		for (Patch patch : this.patches) {
			patch.setNeighbours(this.patches);
		}

		// Add person objects to patches.
		this.populate();
	}

	// Interface: Simulation.

	@Override
	public Output getOutput() {
		// Generate expected simulation output.
		return this.generateOutput();
	}

	@Override
	public void run() {
		List<Thread> threads = new LinkedList<Thread>();
		// Create threads for patches and run them.
		for (Patch patch : this.patches) {
			Thread t = new Thread(patch);
			t.start();
			threads.add(t);
		}

		// Wait until all threads finished.
		for (Thread thread : threads) {
			try {
				thread.join();
			} catch (InterruptedException e) {
				// No interrupt called in project -> can't get here.
				e.printStackTrace();
			}
		}
	}

	// Private methods.

	/**
	 * This method creates the needed patch object for the given scenario.
	 */
	private void createPatches(int k, int padding, Validator validator) {
		// Split grid into patches.
		Iterator<Rectangle> patches = Utils.getPatches(this.scenario);
		// Create unique id for a patch.
		int i = 0;
		while (patches.hasNext()) {
			Patch patch = new Patch(i, k, padding, patches.next(), scenario, validator);
			this.patches.add(patch);
			i++;
		}
	}

	/**
	 * This method creates all person objects and passes to
	 * the correct patch.
	 */
	private void populate() {
		// Create unique id for a person.
		int id = 0;
		for (PersonInfo personInfo : this.scenario.getPopulation()) {
			for (Patch patch : this.patches) {
				// Check if person is on patch.
				if (patch.getMainGrid().contains(personInfo.getPosition())) {
					// Create person.
					Person p = new Person(id, patch, this.scenario.getParameters(), personInfo);
					// Add person on patch.
					patch.addPerson(p);
					id++;
				}
			}
		}
	}

	/**
	 * This method merges all needed outputs together.
	 */
	private Output generateOutput() {
		// Init variables.
		List<TraceEntry> trace = new LinkedList<TraceEntry>();
		Map<String, List<Statistics>> statistics = new HashMap<String, List<Statistics>>();

		// Merge traces.
		if (this.scenario.getTrace()) {
			for (int i = 0; i <= this.scenario.getTicks(); i++) {
				Map<Integer,PersonInfo> map = new HashMap<Integer,PersonInfo>();
				List<PersonInfo> list = new LinkedList<PersonInfo>();
				// Merge all maps
				for (Patch patch : this.patches) {
					map.putAll(patch.getPopulationTrace(i));
				}
				// Add personInfo in correct order to list.
				for (int j= 0; j < this.scenario.getPopulation().size(); j++) {
					list.add(map.get(j));
				}
				// Add new trace entry.
				trace.add(new TraceEntry(list));
			}
		}

		// Merge Statistics.
		for (Patch patch : this.patches) {
			// Just safe first patch.
			if (statistics.isEmpty()) {
				statistics = patch.getStatistics();
				continue;
			}
			// Merge statistics.
			for (Map.Entry<String,Query> entry : this.scenario.getQueries().entrySet()) {
				List<Statistics> ostats = patch.getStatistics().get(entry.getKey());
				List<Statistics> stats = statistics.get(entry.getKey());
				for (int i = 0; i <= this.scenario.getTicks(); i++) {
					long infected = ostats.get(i).getInfected() + stats.get(i).getInfected();
					long infectious = ostats.get(i).getInfectious() + stats.get(i).getInfectious();
					long recovered = ostats.get(i).getRecovered() + stats.get(i).getRecovered();
					long susceptible = ostats.get(i).getSusceptible() + stats.get(i).getSusceptible();
					Statistics s = new Statistics(susceptible, infected, infectious, recovered);
					statistics.get(entry.getKey()).remove(i);
					statistics.get(entry.getKey()).add(i, s);
				}
			}
		}
		return new Output(this.scenario, trace, statistics);
	}

	/**
	 * This methods calculates k.
	 */
	private int calculateK(int padding) throws InsufficientPaddingException {
		// Movement Uncertainty: k+1
		// Infection Uncertainty: incRadius * (1+(k/(incTime+1)))
		// Overall Uncertainty: "sum is a safe overapproximation"
		// Source: https://np20.pseuco.com/t/hints-on-the-spread-of-uncertainty/
		int incTime = this.scenario.getParameters().getIncubationTime();
		int incRadius = this.scenario.getParameters().getInfectionRadius();

		int k = 1;
		// Increase until formula is larger then padding.
		while (( (2*k) + (incRadius * (1+((k-1)/incTime))) ) <= padding)
			k++;

		// Check sufficient padding.
		if (k == 1)
			throw new InsufficientPaddingException(padding);

		// Return last k which was smaller or equal padding.
		return k-1;
	}
}
