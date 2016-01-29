var util = require('util');
var bleno = require('bleno');
var BlenoCharacteristic = bleno.Characteristic;
var BlenoDescriptor = bleno.Descriptor;
var bc = require('./BluetoothConstants');

function SteerCharacteristic(model) {
	SteerCharacteristic.super_.call(this, {
        
		uuid: bc.BluetoothConstants.CHARACTERISTIC_STEER_GUID,
		properties: ['write', 'writeWithoutResponse', 'read'],
		descriptors: [
			new BlenoDescriptor({
				uuid: '2901',
				value: 'Steering'
			})
		]
	});
    this.model = model;
}

util.inherits(SteerCharacteristic, BlenoCharacteristic);

SteerCharacteristic.prototype.onWriteRequest = function (data, offset, withoutResponse, callback) {
	console.log("Received bluetooth data (" + data.length + ")");
	if (offset) {
		callback(this.RESULT_ATTR_NOT_LONG);
	} else if (data.length !== 1) {
		callback(this.RESULT_INVALID_ATTRIBUTE_LENGTH);
	} else {
        var percent = data.readInt8(0);
		console.log("Received Bluetooth (Steering): " + percent + "%");
		this.model.setSteering(percent);
		callback(this.RESULT_SUCCESS);
	}
};

SteerCharacteristic.prototype.onReadRequest = function(offset, callback) {
    if(offset) {
        callback(this.RESULT_ATTR_NOT_LONG, null);
    } else {
        var m = this.model;
        var data = new Buffer(1);
        console.log("Received read request (Steering). Sending " + m.getSteering() + "%");
        data.writeInt8(this.model.getSteering(), 0);
        callback(this.RESULT_SUCCESS, data);
    }
}

module.exports = SteerCharacteristic;