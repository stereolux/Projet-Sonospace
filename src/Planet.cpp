#include "Planet.h"

const int MIN_MEASURE = 120;
const int MAX_MEASURE = 550;
const int MIN_MAP = 0;
const int MAX_MAP = 127;
const int NUMBER_OF_MEASURES = 10;

Planet::Planet(int irPin, int ledPin, int controller) : _irPin(irPin), _ledPin(ledPin), _controller(controller) {
	pinMode(irPin, INPUT);
	pinMode(ledPin, OUTPUT);
}

int Planet::getIrPin() {
	return _irPin;
}

int Planet::getLedPin() {
	return _ledPin;
}

int Planet::getController() {
	return _controller;
}

int Planet::computeValue() {
	int min = 1024;
	int max = 0;
	int sum = 0;
	// make multiple measures to get a mean value
	// more accurate than a single one
	for (int i = 0; i < NUMBER_OF_MEASURES; i++) {
		int val = analogRead(_irPin);
		delay(2);
		// keep min and max measures to supress them from the calculation
		if (val < min) min = val;
		if (val > max) max = val;
		sum += val;
	}
	// compute mean value
	int meanVal = int((sum - (min + max)) / (NUMBER_OF_MEASURES - 2));
	// when out of range of the sensor make the returned value as 0
	if (meanVal < MIN_MEASURE) {
		meanVal = MAX_MEASURE;
	}
	// constrain it between the min and max boundaries
	int adjustedVal = constrain(meanVal, MIN_MEASURE, MAX_MEASURE);
	// lowest value means obstacle is far so we send a big value (and vice versa)
	adjustedVal = map(adjustedVal, MIN_MEASURE, MAX_MEASURE, MAX_MAP, MIN_MAP);
	return adjustedVal;
}

void Planet::logValue() {
	int currentValue = computeValue();
	Serial.print("Raw value: ");
	Serial.print(analogRead(_irPin));
	Serial.print(", Controller: ");
	Serial.print(_controller);
	Serial.print(", Value: ");
	Serial.println(currentValue);
}

void Planet::sendControlChange() {
	int currentValue = computeValue();
	MIDI.sendControlChange(_controller, currentValue, 1);
}

void Planet::setBrightness(int midiValue) {
	// MIDI control change messages are from 0 to 127
	// and brightness (PWM) is from 0 to 255
	int brightness = map(midiValue, 0, 127, 0, 255);
	analogWrite(_ledPin, brightness);
}