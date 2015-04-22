#ifndef Planet_H
#define Planet_H

#include <Arduino.h>
#include "RunningMedian.h"


class Planet {
	private:
		int _irPin;
		int _ledPin;
		RunningMedian _smoother;

	public:
		Planet(int irPin, int ledPin);
		int getIrPin();
		int getLedPin();
		int computeValue();
		void logValue();
		void setBrightness(int midiValue);
		void addValue();
};

#endif