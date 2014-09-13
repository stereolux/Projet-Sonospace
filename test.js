var midi = require('midi');

// Set up a new output.
var output = new midi.output();

output.openPort(1);

var i = 0;

var sendMsg = function() {
	output.sendMessage([176,12,i]);
	console.log(i);
	i++;
	if (i > 127) i = 0;
	console.log(i);
	setTimeout(sendMsg, 200);
};

sendMsg();




//output.closePort();