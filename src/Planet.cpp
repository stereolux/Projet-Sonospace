#include "Planet.h"

const int MIN_MEASURE = 10;
const int MAX_MEASURE = 60;
const int MIN_MAP = 0;
const int MAX_MAP = 127;
const int NUMBER_OF_MEASURES = 50;

Planet::Planet(int irPin, int ledPin, int controller) : _irPin(irPin), _ledPin(ledPin), _controller(controller), _previousValue(0), _currentBrightness(0), _smoother(49) {
	pinMode(irPin, INPUT);
	pinMode(ledPin, OUTPUT);
	analogWrite(ledPin, 0);
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

/*int Planet::computeValue() {
	int min = 1024;
	int max = 0;
	int sum = 0;
	// make multiple measures to get a mean value
	// more accurate than a single one
	for (int i = 0; i < NUMBER_OF_MEASURES; i++) {
		int val = analogRead(_irPin);
		// keep min and max measures to supress them from the calculation
		if (val < min) min = val;
		if (val > max) max = val;
		sum += val;
		// delay between readings for stability
		//delay(2);
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
}*/

int Planet::computeValue() {
	// compute mean value
	int meanVal = (int) _smoother.getAverage(5);
	// convert it to centimeters
	float cm = pow(3027.4/meanVal, 1.2134);
	// if lower than the MIN_MEASURE, convert it to MIN_MEASURE
	if (cm < MIN_MEASURE) cm = MIN_MEASURE;
	// if greater to the MAX_MEASURE, convert it to MAX_MEASURE
	if (cm > MAX_MEASURE) cm = MAX_MEASURE;
	// constrain it between the min and max boundaries
	// lowest value means obstacle is far so we send a big value (and vice versa)
	int midi = map(cm, MIN_MEASURE, MAX_MEASURE, MIN_MAP, MAX_MAP);
	return midi;
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
	// only send MIDI control change if value changed
	if (currentValue != NAN && currentValue != _previousValue) {
		_previousValue = currentValue;
		//if (_currentBrightness > 0) {
			MIDI.sendControlChange(_controller, currentValue, 11);
		//}
	}
}

void Planet::setBrightness(int midiValue) {
	// MIDI control change messages are from 0 to 127
	// and brightness (PWM) is from 0 to 255
	int brightness = map(midiValue, 0, 127, 0, 255);
	_currentBrightness = brightness;
	analogWrite(_ledPin, brightness);
}

void Planet::addValue() {
	// add value to the smoother
	int value = analogRead(_irPin);
	_smoother.add(value);
}
