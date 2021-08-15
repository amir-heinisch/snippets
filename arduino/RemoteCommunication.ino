/*
 * filename:	RemoteCommunication.ino
 * author:	Amir Heinisch <mail@amir-heinisch.de>
 * version:	16/08/2021
 */

#include "defs.h"

// Button input.
const int BTN = 3;
volatile int btn_press_begin, btn_pause_begin = 0;
const int LED_INP_STATE = 2;

// LED binary (output).
const int LED_BIT_BASE = 5;
const int LED_BIT_MAX = 11;

void set_leds_for_seq(String ltr) {
	// Set leds in binary for index.
	int n = mapping_idx_for_letter(ltr.c_str()) + 1;
	if (n != -1) {
		for (int i = LED_BIT_BASE; i <= LED_BIT_MAX; i++) {
			if (n % 2 == 1) {
				digitalWrite(i, HIGH);
			} else {
				digitalWrite(i, LOW);
			}
			n = n / 2;
		}
	}
}

/* IRQ handler */

void btn_changed() {
	if (digitalRead(BTN) == HIGH) { // transition from LOW
		btn_pause_begin = 0;
		btn_press_begin = millis();
		digitalWrite(LED_INP_STATE, HIGH);
	} else {
		int pressed_time = millis() - btn_press_begin; // calc press length.
		btn_press_begin = 0; // reset last press begin.

		btn_pause_begin = millis(); // begin pause now.
		digitalWrite(LED_INP_STATE, LOW); // disable led.

		// Evaluate press length.
		if (pressed_time >= LONG_PRESS) {
			morse_code += "-";
		} else if (pressed_time > MIN_PRESS) {
			morse_code += ".";
		}

		// Serial.print(morse_code.c_str());
		// Serial.print("\n");
	}
}

/* Main arduino methods */

void setup() {
	Serial.begin(9600);

	// Button input method.
	pinMode(BTN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(BTN), btn_changed, CHANGE);
	pinMode(LED_INP_STATE, OUTPUT);
	digitalWrite(LED_INP_STATE, LOW);

	// Binary led output method.
	for (int i = LED_BIT_BASE; i <= LED_BIT_MAX; i++) {
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}

	Serial.print("Setup done!\n");
}

void loop() {
	delay(MAIN_LOOP_DELAY);

	// Output serial message if available.
	if (Serial.available()) {
		// Read from serial.
		String msg = Serial.readStringUntil('\r');
		msg.toUpperCase();

		for (int i = 0; i < msg.length(); i++) {
			// Output to motor.
			set_leds_for_seq(msg.substring(i, i+1));
			delay(3000);
		}
	}

	// Output button input to serial if available.
	int pause_time = millis() - btn_pause_begin;
	if (btn_pause_begin > 0 && pause_time >= LONG_PAUSE && morse_code != "") {
		// Visually indicate long pause.
		digitalWrite(LED_INP_STATE, HIGH);
		delay(10);
		digitalWrite(LED_INP_STATE, LOW);
		// Output to serial.
		Serial.write(letter_for_seq(morse_code.c_str()));
		// Reset.
		btn_pause_begin = 0;
		morse_code = "";
	}
}
