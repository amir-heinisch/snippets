// Import brain utility stuff.
import saarland.cispa.sopra.ki.BrainUtility.Orientation;
import saarland.cispa.sopra.ki.BrainUtility.Direction;
import saarland.cispa.sopra.ki.BrainUtility.SenseAction;
import saarland.cispa.sopra.ki.BrainUtility.JumpMarkInterface;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Example {

    /**
     * Here you first need to add all jump marks you want to use.
     */
    public enum M implements JumpMarkInterface {
        ENTRYPOINT, ENTRYPOINT1, ENTRYPOINT2, ENTRYPOINT3,

        PICKUPFOOD, PICKUPFOOD1, PICKUPFOOD2, PICKUPFOOD3, PICKUPFOOD4,

        MOVEANDMARK, MOVEANDMARK1, MOVEANDMARK2, MOVEANDMARK3, MOVEANDMARK4, MOVEANDMARK5, MOVEANDMARK6,

        RANDOMMOVE,

        DROPFOOD, DROPFOOD1,
        DROPFOODANDGETMORE, DROPFOODANDGETMORE1,

        RANDOMMOVENOTAHEAD, RANDOMMOVENOTAHEAD1, RANDOMMOVENOTAHEAD2, RANDOMMOVENOTAHEAD3, RANDOMMOVENOTAHEAD4, RANDOMMOVENOTAHEAD5,
        MOVEINRANDOMDIRECTIONPRIOAHEAD,
        MOVEINRANDOMDIRECTION,

        MAKEFOODSTREET, MAKEFOODSTREET1, MAKEFOODSTREET2, MAKEFOODSTREET3, MAKEFOODSTREET4, MAKEFOODSTREET5, MAKEFOODSTREET6,

        DELETEFOODSTREET, DELETEFOODSTREET1, DELETEFOODSTREET2, DELETEFOODSTREET3, DELETEFOODSTREET4, DELETEFOODSTREET5, DELETEFOODSTREET6,

        FOLLOWFOODSTREET, FOLLOWFOODSTREET1, FOLLOWFOODSTREET2, FOLLOWFOODSTREET3, FOLLOWFOODSTREET4, FOLLOWFOODSTREET5, FOLLOWFOODSTREET6,

        MAKEFOODSTREET_TURNTONORTHWESTANDMOVE,
        MAKEFOODSTREET_TURNTONORTHEASTANDMOVE,
        MAKEFOODSTREET_TURNTOWESTANDMOVE,
        MAKEFOODSTREET_TURNTOEASTANDMOVE,
        MAKEFOODSTREET_TURNTOSOUTHWESTANDMOVE,
        MAKEFOODSTREET_TURNTOSOUTHEASTANDMOVE,

        TURNTONORTHWESTANDMOVE,
        TURNTONORTHEASTANDMOVE,
        TURNTOWESTANDMOVE,
        TURNTOEASTANDMOVE,
        TURNTOSOUTHWESTANDMOVE,
        TURNTOSOUTHEASTANDMOVE,

        MOVENOTPOSSIBLENORTHEAST,
        MOVENOTPOSSIBLENORTHWEST,
        MOVENOTPOSSIBLEEAST,
        MOVENOTPOSSIBLEWEST,
        MOVENOTPOSSIBLESOUTHEAST,
        MOVENOTPOSSIBLESOUTHWEST,
    }

    public static void main(String[] args) {
        // Create ant.
        BrainUtility.Ant ant = BrainUtility.createAnt("example");
        // Create brain for ant.
        buildBrain(ant);
        // Get brain.
        String brain = BrainUtility.createBrain(ant);

        // Write brain to file.
        try {
            String path = System.getProperty("user.dir") + "/example.brain";
            Files.write(Paths.get(path), brain.getBytes());
        } catch (IOException e) {
            throw new IllegalArgumentException("Could not write brain to file.", e);
        }
    }

    public static void buildBrain(BrainUtility.Ant ant) {
        ant
            // @ENTRYPOINT
            .sense(M.ENTRYPOINT, Orientation.AHEAD, SenseAction.FOOD, M.ENTRYPOINT1)
            .jump(M.PICKUPFOOD)
            // @ENTRYPOINT1
            .turn(M.ENTRYPOINT1, Orientation.RIGHT)
            .direction(Direction.NORTHWEST, M.ENTRYPOINT2)
            .jump(M.ENTRYPOINT3)
            // @ENTRYPOINT2
            .sense(M.ENTRYPOINT2, Orientation.AHEAD, SenseAction.FOOD, M.ENTRYPOINT1)
            .jump(M.PICKUPFOOD)
            // @ENTRYPOINT3
            .sense(M.ENTRYPOINT3, Orientation.AHEAD, SenseAction.HOME, M.MOVEANDMARK)
            .turn(Orientation.RIGHT)
            .direction(Direction.NORTHWEST, M.ENTRYPOINT3)
            .jump(M.RANDOMMOVE)

            // @PICKUPFOOD
            .sense(M.PICKUPFOOD, Orientation.AHEAD, SenseAction.ANTLION, M.PICKUPFOOD1)
            .jump(M.RANDOMMOVENOTAHEAD)
            // @PICKUPFOOD1
            .move(M.PICKUPFOOD1, M.PICKUPFOOD3)
            .pickup(M.RANDOMMOVE)
            .turn(Orientation.LEFT)
            .turn(Orientation.LEFT)
            .turn(Orientation.LEFT)
            .sense(Orientation.HERE, SenseAction.FOOD, M.PICKUPFOOD4)
            .jump(M.PICKUPFOOD2)
            // @PICKUPFOOD2
            .move(M.PICKUPFOOD2, M.PICKUPFOOD2)
            .jump(M.MAKEFOODSTREET)
            // @PICKUPFOOD3
            .sense(M.PICKUPFOOD3, Orientation.AHEAD, SenseAction.FOOD, M.RANDOMMOVE)
            .jump(M.PICKUPFOOD)
            // @PICKUPFOOD4
            .move(M.PICKUPFOOD4, M.PICKUPFOOD4)
            .jump(M.DELETEFOODSTREET)

            // @MOVEANDMARK
            .sense(M.MOVEANDMARK, Orientation.AHEAD, SenseAction.ANTLION, M.MOVEANDMARK1)
            .sense(Orientation.AHEAD, SenseAction.MARKER0, M.MOVEANDMARK1)
            .sense(Orientation.AHEAD, SenseAction.MARKER1, M.MOVEANDMARK1)
            .sense(Orientation.AHEAD, SenseAction.MARKER2, M.MOVEANDMARK1)
            .sense(Orientation.AHEAD, SenseAction.MARKER3, M.MOVEANDMARK1)
            .sense(Orientation.AHEAD, SenseAction.MARKER4, M.MOVEANDMARK1)
            .sense(Orientation.AHEAD, SenseAction.MARKER5, M.MOVEANDMARK1)
            .jump(M.RANDOMMOVENOTAHEAD)
            // @MOVEANDMARK1
            .direction(M.MOVEANDMARK1, Direction.NORTHWEST, M.MOVEANDMARK2)
            .move(M.MOVENOTPOSSIBLENORTHWEST)
            .mark(0)
            .jump(M.MOVEANDMARK)
            // @MOVEANDMARK2
            .direction(M.MOVEANDMARK2, Direction.NORTHEAST, M.MOVEANDMARK3)
            .move(M.MOVENOTPOSSIBLENORTHEAST)
            .mark(1)
            .move(M.MOVEANDMARK)
            // @MOVEANDMARK3
            .direction(M.MOVEANDMARK3, Direction.EAST, M.MOVEANDMARK4)
            .move(M.MOVENOTPOSSIBLEEAST)
            .mark(2)
            .move(M.MOVEANDMARK)
            // @MOVEANDMARK4
            .direction(M.MOVEANDMARK4, Direction.SOUTHEAST, M.MOVEANDMARK5)
            .move(M.MOVENOTPOSSIBLESOUTHEAST)
            .mark(3)
            .move(M.MOVEANDMARK)
            // @MOVEANDMARK5
            .direction(M.MOVEANDMARK5, Direction.SOUTHWEST, M.MOVEANDMARK6)
            .move(M.MOVENOTPOSSIBLENORTHEAST)
            .mark(4)
            .move(M.MOVEANDMARK)
            // @MOVEANDMARK6
            .direction(M.MOVEANDMARK6, Direction.WEST, M.MOVEANDMARK)
            .move(M.MOVENOTPOSSIBLEWEST)
            .mark(5)
            .move(M.MOVEANDMARK)

            // @RANDOMMOVE
            .move(M.RANDOMMOVE, 4)

            // @RANDOMMOVENOTAHEAD
            .sense(M.RANDOMMOVENOTAHEAD, Orientation.LEFT, SenseAction.FOOD, M.RANDOMMOVENOTAHEAD1)
            .turn(Orientation.LEFT)
            .jump(M.PICKUPFOOD)
            // @RANDOMMOVENOTAHEAD1
            .sense(M.RANDOMMOVENOTAHEAD1, Orientation.RIGHT, SenseAction.FOOD, M.RANDOMMOVENOTAHEAD2)
            .turn(Orientation.RIGHT)
            .jump(M.PICKUPFOOD)
            // @RANDOMMOVENOTAHEAD2
            .sense(M.RANDOMMOVENOTAHEAD2, Orientation.LEFT, SenseAction.MARKER6, M.RANDOMMOVENOTAHEAD3)
            .turn(Orientation.LEFT)
            .jump(M.FOLLOWFOODSTREET)
            // @RANDOMMOVENOTAHEAD3
            .sense(M.RANDOMMOVENOTAHEAD3, Orientation.RIGHT, SenseAction.MARKER6, M.RANDOMMOVENOTAHEAD4)
            .turn(Orientation.RIGHT)
            .jump(M.FOLLOWFOODSTREET)
            // @RANDOMMOVENOTAHEAD4
            .jump(M.RANDOMMOVENOTAHEAD4, M.RANDOMMOVE)

            // @MOVENOTPOSSIBLEWEST
            .move(M.MOVENOTPOSSIBLEWEST, M.MOVENOTPOSSIBLEWEST)
            .move(M.MOVENOTPOSSIBLEEAST, M.MOVENOTPOSSIBLEEAST)
            .move(M.MOVENOTPOSSIBLESOUTHWEST, M.MOVENOTPOSSIBLESOUTHWEST)
            .move(M.MOVENOTPOSSIBLESOUTHEAST, M.MOVENOTPOSSIBLESOUTHEAST)
            .move(M.MOVENOTPOSSIBLENORTHWEST, M.MOVENOTPOSSIBLENORTHWEST)
            .move(M.MOVENOTPOSSIBLENORTHEAST, M.MOVENOTPOSSIBLENORTHEAST)

            // @MAKEFOODSTREET
            .sense(M.MAKEFOODSTREET, Orientation.HERE, SenseAction.HOME, M.MAKEFOODSTREET1)
            .jump(M.DROPFOODANDGETMORE)
            // @MAKEFOODSTREET1
            .mark(M.MAKEFOODSTREET1, 6)
            .sense(Orientation.HERE, SenseAction.MARKER0, M.MAKEFOODSTREET2)
            .jump(M.MAKEFOODSTREET_TURNTOSOUTHEASTANDMOVE)
            // @MAKEFOODSTREET2
            .sense(M.DELETEFOODSTREET2, Orientation.HERE, SenseAction.MARKER1, M.DELETEFOODSTREET3)
            .jump(M.MAKEFOODSTREET_TURNTOSOUTHWESTANDMOVE)
            // @MAKEFOODSTREET3
            .sense(M.DELETEFOODSTREET3, Orientation.HERE, SenseAction.MARKER2, M.DELETEFOODSTREET4)
            .jump(M.MAKEFOODSTREET_TURNTOWESTANDMOVE)
            // @MAKEFOODSTREET4
            .sense(M.DELETEFOODSTREET4, Orientation.HERE, SenseAction.MARKER3, M.DELETEFOODSTREET5)
            .jump(M.MAKEFOODSTREET_TURNTONORTHWESTANDMOVE)
            // @MAKEFOODSTREET5
            .sense(M.DELETEFOODSTREET5, Orientation.HERE, SenseAction.MARKER4, M.DELETEFOODSTREET6)
            .jump(M.MAKEFOODSTREET_TURNTONORTHEASTANDMOVE)
            // @MAKEFOODSTREET6
            .sense(M.DELETEFOODSTREET6, Orientation.HERE, SenseAction.MARKER5, M.RANDOMMOVE) // Ant completely lost track.
            .jump(M.MAKEFOODSTREET_TURNTOEASTANDMOVE)

            // @DELETEFOODSTREET
            .sense(M.DELETEFOODSTREET, Orientation.HERE, SenseAction.HOME, M.DELETEFOODSTREET1)
            .jump(M.DROPFOOD)
            // @DELETEFOODSTREET1
            .unmark(M.DELETEFOODSTREET1, 6)
            .sense(Orientation.HERE, SenseAction.MARKER0, M.DELETEFOODSTREET2)
            .jump(M.TURNTOSOUTHEASTANDMOVE)
            // @DELETEFOODSTREET2
            .sense(M.DELETEFOODSTREET2, Orientation.HERE, SenseAction.MARKER1, M.DELETEFOODSTREET3)
            .jump(M.TURNTOSOUTHWESTANDMOVE)
            // @DELETEFOODSTREET3
            .sense(M.DELETEFOODSTREET3, Orientation.HERE, SenseAction.MARKER2, M.DELETEFOODSTREET4)
            .jump(M.TURNTOWESTANDMOVE)
            // @DELETEFOODSTREET4
            .sense(M.DELETEFOODSTREET4, Orientation.HERE, SenseAction.MARKER3, M.DELETEFOODSTREET5)
            .jump(M.TURNTONORTHWESTANDMOVE)
            // @DELETEFOODSTREET5
            .sense(M.DELETEFOODSTREET5, Orientation.HERE, SenseAction.MARKER4, M.DELETEFOODSTREET6)
            .jump(M.TURNTONORTHEASTANDMOVE)
            // @DELETEFOODSTREET6
            .sense(M.DELETEFOODSTREET6, Orientation.HERE, SenseAction.MARKER5, M.RANDOMMOVE) // Ant completely lost track.
            .jump(M.TURNTOEASTANDMOVE)

            // @FOLLOWFOODSTREET
            .sense(M.FOLLOWFOODSTREET, Orientation.AHEAD, SenseAction.FOOD, M.FOLLOWFOODSTREET1)
            .jump(M.PICKUPFOOD)
            // @FOLLOWFOODSTREET1
            .sense(M.FOLLOWFOODSTREET1, Orientation.AHEAD, SenseAction.MARKER6, M.FOLLOWFOODSTREET2)
            .jump(M.MOVEANDMARK)

            // @DROPFOOD
            .drop(M.DROPFOOD, M.DROPFOOD1)
            // @DROPFOOD1
            .jump(M.DROPFOOD1, M.RANDOMMOVE)

            // @DROPFOODANDGETMORE
            .drop(M.DROPFOODANDGETMORE, M.DROPFOODANDGETMORE1)
            // @DROPFOODANDGETMORE1
            .turn(M.DROPFOODANDGETMORE1, Orientation.LEFT)
            .turn(Orientation.LEFT)
            .turn(Orientation.LEFT)
            .jump(M.FOLLOWFOODSTREET)
            ;
   }
}
