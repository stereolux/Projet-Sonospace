#include <SoftwareSerial.h>
#include <XBee.h>
#include <Planet.h>
#include <elapsedMillis.h>

// planet conf
Planet planet(A0, 3);

// keep track of previous IR distance sensed
int oldCm = 0;

// interval timing
elapsedMillis timeElapsed;
unsigned int interval = 100;

// xbee conf
// software serial port for xbee (rx 10, tx 11)
SoftwareSerial xbeeSerial(10, 11);
XBee xbee = XBee();
Rx16Response rx16 = Rx16Response();
Tx16Request tx;
uint8_t payload[] = {0};
// xbee destination adress (i.e. computer)
uint16_t adress = 0x0009;

void setup() {
	Serial.begin(9600);
	xbeeSerial.begin(9600);
	xbee.setSerial(xbeeSerial);
}

void loop() {
	// check for new xbee data
	int brightness = getNewData();
	if (brightness != -1) {
		//Serial.println(brightness);
		planet.setBrightness(brightness);
	}
	// add a new sample to the smoother
	planet.addValue();
	// send sensor data if elapsed time is more than the interval
	if (timeElapsed > interval) {
		int cm = planet.computeValue();
		// if value changed, send it
		if (cm != NAN && cm != oldCm) {
			oldCm = cm;
			payload[0] = cm;
			tx = Tx16Request(adress, payload, sizeof(payload));
			xbee.send(tx);
		}
		timeElapsed = 0;
	}
}

// check and return new data from xbee if there is some, or NAN if no new data
int getNewData() {
	xbee.readPacket();
	if (xbee.getResponse().isAvailable()) {
		if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
			// if the xbee packet we received is ok, get the data
			xbee.getResponse().getRx16Response(rx16);
			int respLength = rx16.getDataLength();
			char value[respLength + 1];
			for(int i=0; i < respLength; i++){
				value[i] = rx16.getData(i);
			}
			value[respLength] = 0;
			return atoi(value);
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
}