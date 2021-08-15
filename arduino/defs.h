/*
 * filename:	defs.h
 * author:	Amir Heinisch <mail@amir-heinisch.de>
 * version:	16/08/2021
 */

String morse_code = ""; // Use c++ str for ease.

typedef struct { const char *letter; const char *seq; } t_morse_mapping;
static t_morse_mapping morse_mapping[] = {
	{ "A", ".-"    }, { "B", "-..."  }, { "C", "-.-."  },
	{ "D", "-.."   }, { "E", "."     }, { "F", "..-."  },
	{ "G", "--."   }, { "H", "...."  }, { "I", ".."    },
	{ "J", ".---"  }, { "K", "-.-"   }, { "L", ".-.."  },
	{ "M", "--"    }, { "N", "-."    }, { "O", "---"   },
	{ "P", ".--."  }, { "Q", "--.-"  }, { "R", ".-."   },
	{ "S", "..."   }, { "T", "-"     }, { "U", "..-"   },
	{ "V", "...-"  }, { "W", ".--"   }, { "X", "-..-"  },
	{ "Y", "-.--"  }, { "Z", "--.."  }, { " ", "-...-" },
	{ "0", "-----" }, { "1", ".----" }, { "2", "..---" },
	{ "3", "...--" }, { "4", "...-"  }, { "5", "....." },
	{ "6", "-...." }, { "7", "--..." }, { "8", "---.." },
	{ "9", "----." },
};
#define MORSE_LEN (sizeof(morse_mapping)/sizeof(t_morse_mapping))

#define MAIN_LOOP_DELAY 100

#define MIN_PRESS 10
#define LONG_PRESS 300
#define LONG_PAUSE 1500

#define MIN_MOTOR_ANGLE 25
#define MAX_MOTOR_ANGLE 160

int mapping_idx_for_seq(const char *seq) {
	for (int i = 0; i < MORSE_LEN; i++) {
		t_morse_mapping *morse = &morse_mapping[i];
		if (strcmp(morse->seq, seq) == 0) {
			return i;
		}
	}
	return -1;
}

int mapping_idx_for_letter(const char *ltr) {
  for (int i = 0; i < MORSE_LEN; i++) {
    t_morse_mapping *morse = &morse_mapping[i];
    if (strcmp(morse->letter, ltr) == 0) {
      return i;
    }
  }
  return -1;
}

t_morse_mapping *mapping_for_seq(const char *seq) {
	int idx = mapping_idx_for_seq(seq);
	if (idx >= 0 && idx < MORSE_LEN) {
		return &morse_mapping[idx];
	}
	return NULL;
}

const char *letter_for_seq(const char *seq) {
	t_morse_mapping *mapping = mapping_for_seq(seq);
	return mapping ? mapping->letter : "FAILED";
}
