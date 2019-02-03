import java.util.ArrayList;
import java.util.List;

/**
 * This class should be used to create brains faster.
 *
 * Use this utility like this:
 *
 * // Create new ant.
 * Ant ant = BrainUtility.createAnt("example");
 *
 * // Call instructions on ant.
 * ant.move(2).turn(Direction.LEFT).turn(Direction.RIGHT).mark(09.unmark(0)
 *    .set(0).unset(0).test(0).sense();
 *
 * // Create brain from ant.
 * String brain = BrainUtility.createBrain(ant);
 *
 */
public final class BrainUtility {

    /**
     * Here you first need to add all jump marks you want to use.
     */
    public enum JumpMark {}

    /**
     * This enum contains all directions.
     */
    public enum Direction {
        NORTHWEST,
        NORTHEAST,
        EAST,
        SOUTHEAST,
        SOUTHWEST,
        WEST;
    }

    /**
     * This enum contains all orientations.
     */
    public enum Orientation {
        LEFT,
        RIGHT,
        AHEAD,
        HERE;
    }

    /**
     * This enum contains all sense actions.
     */
    public enum SenseAction {
        FRIEND,
        FOE,
        FOOD,
        ROCK,
        HOME,
        FOEHOME,
        FOEMARKER,
        FRIENDFOOD,
        FOEFOOD,
        ANTLION,
        MARKER0,
        MARKER1,
        MARKER2,
        MARKER3,
        MARKER4,
        MARKER5,
        MARKER6,
    }

    /**
     * This enum contains all possible instructions.
     */
    public enum InstructionType {
        MARK, UNMARK, TEST,
        SET, UNSET,
        PICKUP, DROP,
        TURN, DIRECTION,
        MOVE, SENSE, JUMP, FLIP, BREED;
    }

    /**
     * This class represents an instruction.
     */
    public static class Instruction {
        public JumpMark srcMark;
        public JumpMark dstMark;
        public int elseCase;

        public int index;

        public SenseAction senseAction;
        public Orientation orientation;
        public Direction direction;

        // Indicates if instruction has marks.
        public boolean hasMarks = false;
        // Indicates if instruction marks are already cleared.
        public boolean clearedMarks = false;

        public InstructionType type;

        public Instruction(InstructionType type) {
            this.type = type;
        }

        @Override
        public String toString() {
            String res = "";

            if (this.srcMark != null) {
                res += "\n\t// @" + this.srcMark.toString() + "\n\t";
            }

            switch (this.type) {
                case SET:
                    res += "set " + this.index;
                    break;
                case UNSET:
                    res += "unset " + this.index;
                    break;
                case MARK:
                    res += "mark " + this.index;
                    break;
                case UNMARK:
                    res += "unmark " + this.index;
                    break;
                case TEST:
                    res += "test " + this.index + " else " + this.elseCase;
                    break;
                case DROP:
                    res += "drop else " + this.elseCase;
                    break;
                case PICKUP:
                    res += "pickup else " + this.elseCase;
                    break;
                case FLIP:
                    res += "flip " + this.index + " else " + this.elseCase;
                    break;
                case JUMP:
                    res += "jump " + this.elseCase;
                    break;
                case MOVE:
                    res += "move else " + this.elseCase;
                    break;
                case TURN:
                    res += "turn " + this.orientation.toString().toLowerCase();
                    break;
                case BREED:
                    res += "breed else " + this.elseCase;
                    break;
                case SENSE:
                    String senseAc = "";
                    // @TODO - add whitespace for marker.
                    if (this.senseAction.toString().startsWith("MARKER")) {
                        String tmp = this.senseAction.toString();
                        tmp = tmp.substring(6, tmp.length());
                        senseAc = "marker " + tmp;
                    } else {
                        senseAc = this.senseAction.toString().toLowerCase();
                    }

                    res += "sense " + this.orientation.toString().toLowerCase() + " " + senseAc + " else " + this.elseCase;
                    break;
                case DIRECTION:
                    res += "direction " + this.direction.toString().toLowerCase() + " else " + this.elseCase;
                    break;
                default:
                    throw new IllegalArgumentException("Type unknown.");
            }
            return res;
        }

        public void clearMarks(List<Instruction> instructions) {
            if (!this.hasMarks) {
                return;
            }

            if (this.dstMark != null) {
                for (int i = 0; i < instructions.size(); i++) {
                    if (instructions.get(i).srcMark == this.dstMark) {
                        this.elseCase = i;
                    }
                }
            }

            this.clearedMarks = true;
        }
    }

    /**
     * This class represents an ant.
     */
    public static class Ant  {

        private final String name;
        private final List<Instruction> instructions;

        public Ant(String name) {
            this.name = name;
            this.instructions = new ArrayList<>();
        }

        /**
         * This function returns the brain string of this ant.
         */
        private String getBrain() {
            String res = "";
            res += "brain ";
            res += "\"" + this.name + "\" {\n";

            for (Instruction instr: this.instructions) {
                instr.clearMarks(this.instructions);
                res+= "\t" + instr.toString() + "\n";
            }
            res += "\n\t// @END \n\tjump " + this.instructions.size() + "\n\n";
            res += "}";

            return res;
        }

        /**
         * This function adds an instruction to this ant.
         */
        private void addInstruction(Instruction instruction) {
            this.instructions.add(instruction);
        }

        /* ### Ant Instructions. ### */

        // Move instruction.
        public Ant move(int elseCase) {
            Instruction instr = new Instruction(InstructionType.MOVE);
            instr.elseCase = elseCase;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant move(JumpMark sourceMark, int elseCase) {
            Instruction instr = new Instruction(InstructionType.MOVE);
            instr.srcMark = sourceMark;
            instr.elseCase = elseCase;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant move(JumpMark elseMark) {
            Instruction instr = new Instruction(InstructionType.MOVE);
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant move(JumpMark sourceMark, JumpMark elseMark) {
            Instruction instr = new Instruction(InstructionType.MOVE);
            instr.srcMark = sourceMark;
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Turn instruction.
        public Ant turn(Orientation orientation) {
            if (orientation != Orientation.RIGHT && orientation != Orientation.LEFT) {
                throw new IllegalArgumentException("Orientation is unkown for turn.");
            }

            Instruction instr = new Instruction(InstructionType.TURN);
            instr.orientation = orientation;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant turn(JumpMark sourceMark, Orientation orientation) {
            if (orientation != Orientation.RIGHT && orientation != Orientation.LEFT) {
                throw new IllegalArgumentException("Orientation is unkown for turn.");
            }

            Instruction instr = new Instruction(InstructionType.TURN);
            instr.srcMark = sourceMark;
            instr.orientation = orientation;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Mark instruction.
        public Ant mark(int index) {
            if (index < 0 || index > 6) {
                throw new IllegalArgumentException("Mark index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.MARK);
            instr.index = index;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant mark(JumpMark sourceMark, int index) {
            if (index < 0 || index > 6) {
                throw new IllegalArgumentException("Mark index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.MARK);
            instr.srcMark = sourceMark;
            instr.index = index;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Unmark instruction.
        public Ant unmark(int index) {
            if (index < 0 || index > 6) {
                throw new IllegalArgumentException("Unmark index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.UNMARK);
            instr.index = index;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant unmark(JumpMark sourceMark, int index) {
            if (index < 0 || index > 6) {
                throw new IllegalArgumentException("Unmark index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.UNMARK);
            instr.srcMark = sourceMark;
            instr.index = index;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Set instruction.
        public Ant set(int index) {
            if (index < 0 || index > 5) {
                throw new IllegalArgumentException("Set index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.SET);
            instr.index = index;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant set(JumpMark sourceMark, int index) {
            if (index < 0 || index > 5) {
                throw new IllegalArgumentException("Set index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.SET);
            instr.srcMark = sourceMark;
            instr.index = index;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Unset instruction.
        public Ant unset(int index) {
            if (index < 0 || index > 5) {
                throw new IllegalArgumentException("Unset index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.UNSET);
            instr.index = index;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant unset(JumpMark sourceMark, int index) {
            if (index < 0 || index > 5) {
                throw new IllegalArgumentException("Unset index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.UNSET);
            instr.srcMark = sourceMark;
            instr.index = index;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Test instruction.
        public Ant test(int index, int elseCase) {
            if (index < 0 || index > 5) {
                throw new IllegalArgumentException("Test index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.TEST);
            instr.index = index;
            instr.elseCase = elseCase;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant test(JumpMark sourceMark, int index, int elseCase) {
            if (index < 0 || index > 5) {
                throw new IllegalArgumentException("Test index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.TEST);
            instr.srcMark = sourceMark;
            instr.index = index;
            instr.elseCase = elseCase;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant test(int index, JumpMark elseMark) {
            if (index < 0 || index > 5) {
                throw new IllegalArgumentException("Test index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.TEST);
            instr.index = index;
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant test(JumpMark sourceMark, int index, JumpMark elseMark) {
            if (index < 0 || index > 5) {
                throw new IllegalArgumentException("Test index is not in range.");
            }

            Instruction instr = new Instruction(InstructionType.TEST);
            instr.index = index;
            instr.srcMark = sourceMark;
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Sense instruction.
        public Ant sense(Orientation orientation, SenseAction action, JumpMark elseMarker) {
            Instruction instr = new Instruction(InstructionType.SENSE);
            instr.orientation = orientation;
            instr.senseAction = action;
            instr.dstMark = elseMarker;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant sense(Orientation orientation, SenseAction action, int elseCase) {
            Instruction instr = new Instruction(InstructionType.SENSE);
            instr.orientation = orientation;
            instr.senseAction = action;
            instr.elseCase = elseCase;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant sense(JumpMark sourceMark, Orientation orientation, SenseAction action, int elseCase) {
            Instruction instr = new Instruction(InstructionType.SENSE);
            instr.orientation = orientation;
            instr.senseAction = action;
            instr.elseCase = elseCase;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant sense(JumpMark sourceMark, Orientation orientation, SenseAction action, JumpMark elseMarker) {
            Instruction instr = new Instruction(InstructionType.SENSE);
            instr.orientation = orientation;
            instr.senseAction = action;
            instr.srcMark = sourceMark;
            instr.dstMark = elseMarker;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Direction instruction.
        public Ant direction(Direction direction, int elseCase) {
            Instruction instr = new Instruction(InstructionType.DIRECTION);
            instr.direction = direction;
            instr.elseCase = elseCase;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant direction(Direction direction, JumpMark elseMark) {
            Instruction instr = new Instruction(InstructionType.DIRECTION);
            instr.direction = direction;
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant direction(JumpMark sourceMark, Direction direction, JumpMark elseMark) {
            Instruction instr = new Instruction(InstructionType.DIRECTION);
            instr.direction = direction;
            instr.srcMark = sourceMark;
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Breed instruction.
        public Ant breed() {
            Instruction instr = new Instruction(InstructionType.BREED);
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Flip instruction.
        public Ant flip(int nmr, int elseCase) {
            Instruction instr = new Instruction(InstructionType.FLIP);
            instr.index = nmr;
            instr.elseCase = elseCase;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant flip(int nmr, JumpMark elseMark) {
            Instruction instr = new Instruction(InstructionType.FLIP);
            instr.index = nmr;
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }


        public Ant flip(JumpMark source , int nmr, JumpMark elseMark) {
            Instruction instr = new Instruction(InstructionType.FLIP);
            instr.srcMark = source;
            instr.index = nmr;
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Drop instruction.
        public Ant drop(int elseCase) {
            Instruction instr = new Instruction(InstructionType.DROP);
            instr.elseCase = elseCase;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant drop(JumpMark sourceMark, JumpMark elseMark) {
            Instruction instr = new Instruction(InstructionType.DROP);
            instr.srcMark = sourceMark;
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Pickup instruction.
        public Ant pickup(int elseCase) {
            Instruction instr = new Instruction(InstructionType.PICKUP);
            instr.elseCase = elseCase;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant pickup(JumpMark sourceMark, JumpMark elseMark) {
            Instruction instr = new Instruction(InstructionType.PICKUP);
            instr.srcMark = sourceMark;
            instr.dstMark = elseMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }

        // Jump instruction.
        public Ant jump(int newPC) {
            Instruction instr = new Instruction(InstructionType.JUMP);
            instr.elseCase = newPC;
            instr.hasMarks = false;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant jump(JumpMark newMark) {
            Instruction instr = new Instruction(InstructionType.JUMP);
            instr.dstMark = newMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
        public Ant jump(JumpMark sourceMark, JumpMark newMark) {
            Instruction instr = new Instruction(InstructionType.JUMP);
            instr.srcMark = sourceMark;
            instr.dstMark = newMark;
            instr.hasMarks = true;

            // Add instruction to ant.
            addInstruction(instr);

            return this;
        }
    }

    private BrainUtility() {
        // No construction needed.
    }

    /**
     * This function creates an ant with the given name.
     */
    public static Ant createAnt(String name) {
        return new Ant(name);
    }

    /**
     * This function creates a brain string from a given ant.
     */
    public static String createBrain(Ant ant) {
        return ant.getBrain();
    }
}
