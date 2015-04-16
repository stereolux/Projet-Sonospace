#include "MIDI.h"
#include "midi_Defs.h"
#include "midi_Namespace.h"
#include "midi_Settings.h"
#include "Planet.h"

Planet planet1(A0, 10, 0);
Planet planet2(A1, 9, 1);
Planet planet3(A2, 7, 2);
Planet planet4(A3, 6, 3);
Planet planet5(A4, 5, 4);
Planet planet6(A5, 4, 5);
Planet planet7(A6, 3, 6);
Planet planet8(A7, 2, 7);

//const int NUMBER_OF_PLANETS = 1;
//const int NUMBER_OF_PLANETS = 5;
const int NUMBER_OF_PLANETS = 8;

//Planet planets[NUMBER_OF_PLANETS] = {planet1};
//Planet planets[NUMBER_OF_PLANETS] = {planet1, planet2, planet3, planet4, planet5};
Planet planets[NUMBER_OF_PLANETS] = {planet1, planet2, planet3, planet4, planet5, planet6, planet7, planet8};

// status led
int statusLed = 51;
// play button (53  -----/ ----- GND)
int playButton = 53;
// reset button (49  -----/ ----- GND)
int resetButton = 49;

// states of buttons
int playCurrent = 0;
int playOld = 0;
int resetCurrent = 0;
int resetOld = 0;

// timer
const int INTERVAL = 100;
long previousMillis = 0;

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
	// activate through midi + HIDUINO
	MIDI.begin(MIDI_CHANNEL_OMNI);
	MIDI.setHandleControlChange(HandleControlChange);
}

void loop() {
	MIDI.read();
	checkButtons();
	// get ir sensor values
	for(int j=0; j < NUMBER_OF_PLANETS; j++){
		planets[j].addValue();
		delay(1);
	}
	// send MIDI messages only every INTERVAL ms
	unsigned long currentMillis = millis();
	if(currentMillis - previousMillis > INTERVAL) {
		previousMillis = currentMillis;
		for(int i=0; i < NUMBER_OF_PLANETS; i++){
			planets[i].sendControlChange();
		}
	}
}

void HandleControlChange (byte channel, byte controller, byte value) {
	// channel 12 is for the leds on the planets
	if (channel == 12) {
		int planetIndex = int(controller - 8);
		if (planetIndex >= 0 && planetIndex <= 7) {
			planets[planetIndex].setBrightness(value);
		}
	}
	// channel 13, controller 18 is the status led
	else if (channel == 13) {
		if (controller == 18) {
			if (value == 1) {
				digitalWrite(statusLed, HIGH);
			}
			else if (value == 0) {
				digitalWrite(statusLed, LOW);
			}
		}
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
