#ifndef Planet_H
#define Planet_H

#include <Arduino.h>
#include "MIDI.h"
#include "midi_Defs.h"
#include "midi_Namespace.h"
#include "midi_Settings.h"

class Planet {
	private:
		int _irPin;
		int _ledPin;
		int _controller;

	public:
		Planet(int irPin, int ledPin, int controller);
		int getIrPin();
		int getLedPin();
		int getController();
		int computeValue();
		void logValue();
		void sendControlChange();
		void setBrightness(int midiValue);
};

#endif