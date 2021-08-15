/*
 * filename:	final_submission/RequiredPipeline.ino
 * author:	Amir Heinisch <mail@amir-heinisch.de>
 * version:	16/08/2021
 */

#include "defs.h"

// Button input.
const int BTN = 3;
const int LED_INP_STATE = 2;
volatile int btn_press_begin, btn_pause_begin = 0;

// Motor output.
#include <Servo.h>
Servo myServo;
const int MOTOR = 13;

void set_output() {
	if (morse_code != "") {
		// Serial output.
		Serial.print("Input: ");
		Serial.println(letter_for_seq(morse_code.c_str()));

		motor_show_letter(morse_code.c_str());
	}

	// Reset.
	morse_code = "";
	btn_pause_begin = 0;
}

void motor_show_letter(String seq) {
	// Get morse mapping position.
	int idx = mapping_idx_for_seq(seq.c_str());
	// Calculate hex digets.
	int first_hex = idx / 16;
	int second_hex = idx % 16;
	// Calculate angles.
	int first_angle = map(first_hex, 0, MORSE_LEN, MIN_MOTOR_ANGLE , MAX_MOTOR_ANGLE);
	int second_angle = map(second_hex, 0, MORSE_LEN, MIN_MOTOR_ANGLE , MAX_MOTOR_ANGLE);
	// Turn motor.
	myServo.write(first_angle);
	delay(1000);
	myServo.write(second_angle);
	delay(1000);
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

	pinMode(LED_INP_STATE, OUTPUT);
	digitalWrite(LED_INP_STATE, LOW);

	// Button input method.
	pinMode(BTN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(BTN), btn_changed, CHANGE);

	// Motor output method.
	myServo.attach(MOTOR); // 25 - 160 degree are working.
	myServo.attach(25);
	delay(1000);

	Serial.print("Setup done!\n");
}

void loop() {
	// Output button input to serial if available.
	int pause_time = millis() - btn_pause_begin;
	if (btn_pause_begin > 0 && pause_time >= LONG_PAUSE) {
		// Visually indicate long pause.
		digitalWrite(LED_INP_STATE, HIGH);
		delay(10);
		digitalWrite(LED_INP_STATE, LOW);
		// Set motor.
		set_output();
	}

}
