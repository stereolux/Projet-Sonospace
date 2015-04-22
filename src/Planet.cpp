#include "Planet.h"

const int MIN_MEASURE = 10;
const int MAX_MEASURE = 80;
const int MIN_MAP = 0;
const int MAX_MAP = 127;

Planet::Planet(int irPin, int ledPin) : _irPin(irPin), _ledPin(ledPin), _smoother(49) {
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
	Serial.println(cm);
	int value = map(cm, MIN_MEASURE, MAX_MEASURE, MIN_MAP, MAX_MAP);
	return value;
}

void Planet::logValue() {
	int currentValue = computeValue();
	Serial.print("Raw value: ");
	Serial.print(analogRead(_irPin));
	Serial.print(", Value: ");
	Serial.println(currentValue);
}

void Planet::setBrightness(int midiValue) {
	// MIDI control change messages are from 0 to 127
	// and brightness (PWM) is from 0 to 255
	int brightness = map(midiValue, 0, 127, 0, 255);
	analogWrite(_ledPin, brightness);
}

void Planet::addValue() {
	// add value to the smoother
	int value = analogRead(_irPin);
	_smoother.add(value);
}
