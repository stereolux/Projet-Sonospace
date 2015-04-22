var util = require('util');
	SerialPort = require('serialport').SerialPort,
	xbee_api = require('xbee-api'),
	C = xbee_api.constants,
	midi = require('midi');

var debug = true;
if (!debug) console.log = function() {};

// conf serial port/xbee
var xbeeAPI = new xbee_api.XBeeAPI({
	api_mode: 2
});

var serialPort = new SerialPort('/dev/ttyUSB0', {
	parser: xbeeAPI.rawParser()
});

serialPort.on('open', function() {
	console.log('[INFO] Serveur démarré');
});

// xbee frame template
var frame = {
	type: C.FRAME_TYPE.TX_REQUEST_16,
};

// midi input
var input = new midi.input();
console.log('Listening to ' + input.getPortName(0) + ' ...');

// configure a callback for incoming midi messages
input.on('message', function(deltaTime, message) {
	var midiMessage = message.toString().split(',').map(function(item) {
		return parseInt(item, 10);
	});
	// message for leds
	if (midiMessage[0] === 187 && 7 < midiMessage[1] && midiMessage[1] < 16) {
		var planet = midiMessage[1] - 8;
		frame.destination16 = '000' + planet.toString(10);
		frame.data = midiMessage[2].toString();
		console.log(frame);
		serialPort.write(xbeeAPI.buildFrame(frame));
	}
	//message for status led
	else if (midiMessage[0] === 188 && midiMessage[1] === 18) {
		if (midiMessage[2] === 1) {
			console.log('on');
		}
		else if (midiMessage[2] === 0) {
			console.log('off');
		}
	}
});
// open the port
input.openPort(0);


// midi output
var output = new midi.output();
output.openVirtualPort('Steles');
// intercept xbee msg and route them to midi
xbeeAPI.on('frame_object', function(frame) {
	if (frame.type === C.FRAME_TYPE.RX_PACKET_16) {
		var midiValue = frame.data.readUInt8(0);
		var planet = parseInt(frame.remote16);
		console.log('midiValue: ' + midiValue);
		console.log('planet: ' + planet);
		output.sendMessage([186, planet, midiValue]);
	}
});