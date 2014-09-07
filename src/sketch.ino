#include "MIDI.h"
#include "midi_Defs.h"
#include "midi_Namespace.h"
#include "midi_Settings.h"
#include "Planet.h"

#define DEBUG false

Planet planet1(A0, 10, 0);
Planet planet2(A1, 9, 1);
Planet planet3(A2, 7, 2);
Planet planet4(A3, 6, 3);
Planet planet5(A4, 5, 4);
Planet planet6(A5, 4, 5);
Planet planet7(A6, 3, 6);
Planet planet8(A7, 2, 7);

const int NUMBER_OF_PLANETS = 1;
//const int NUMBER_OF_PLANETS = 5;
//const int NUMBER_OF_PLANETS = 8;

Planet planets[NUMBER_OF_PLANETS] = {planet1};
//Planet planets[NUMBER_OF_PLANETS] = {planet1, planet2, planet3, planet4, planet5};
//Planet planets[NUMBER_OF_PLANETS] = {planet1, planet2, planet3, planet4, planet5, planet6, planet7, planet8};

// status led
int statusLed = 13;
// play button
int playButton = 11;
// reset button
int resetButton = 12;

// states of buttons
int playCurrent = 0;
int playOld = 0;
int resetCurrent = 0;
int resetOld = 0;

void setup() {
	// define status led
	pinMode(statusLed, OUTPUT);
	// define play button and
	// activate internal pull up resistor
	pinMode(playButton, INPUT);
	digitalWrite(playButton,HIGH);
	// define reset button and
	// activate internal pull up resistor
	pinMode(resetButton, INPUT);
	digitalWrite(resetButton,HIGH);
	// 2 modes on through serial for debbugging
	// another through midi + HIDUINO
	if (DEBUG) {
		Serial.begin(9600);
	}
	else {
		MIDI.begin(MIDI_CHANNEL_OMNI);
		MIDI.setHandleControlChange(HandleControlChange);
	}
}

void loop() {
	if (!DEBUG) MIDI.read();

	checkButtons();

	for(int i=0; i < NUMBER_OF_PLANETS; i++){
		if (DEBUG) {
			planets[i].logValue();
		}
		else {
			planets[i].sendControlChange();
		}
	}
	if (DEBUG) delay(1000);
}

void HandleControlChange (byte channel, byte controller, byte value) {
	// controller 18 is for the status led, all others are for leds on the planets
	if (controller == 18) {
		if (value == 1) {
			digitalWrite(statusLed, HIGH);
		}
		else if (value == 0) {
			digitalWrite(statusLed, LOW);
		}
	}
	else {
		int planetIndex = controller - 8;
		planets[planetIndex].setBrightness(value);
	}
}

void checkButtons() {
	// read current values
	playCurrent = digitalRead(playButton);
	resetCurrent = digitalRead(resetButton);
	// the condition means that the value changed from LOW to HIGH and that the status led is on
	if (playCurrent == HIGH && playOld == LOW && digitalRead(statusLed) == HIGH) {
		// send a control change on controller 19 with value 1
		MIDI.sendControlChange(19, 1, 1);
	}
	// update old value with the current one
	playOld = playCurrent;
	// the condition means that the value changed from LOW to HIGH
	if (resetCurrent == HIGH && resetOld == LOW) {
		// send a control change on controller 20 with value 1
		MIDI.sendControlChange(20, 1, 1);
	}
	// update old value with the current one
	resetOld = resetCurrent;
}