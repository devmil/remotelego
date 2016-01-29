var util = require('util');
var bleno = require('bleno');
var BlenoCharacteristic = bleno.Characteristic;
var BlenoDescriptor = bleno.Descriptor;
var bc = require('./BluetoothConstants');

function SpeedCharacteristic(model) {
	SpeedCharacteristic.super_.call(this, {
        
		uuid: bc.BluetoothConstants.CHARACTERISTIC_SPEED_GUID,
		properties: ['write', 'writeWithoutResponse', 'read'],
		descriptors: [
			new BlenoDescriptor({
				uuid: '2901',
				value: 'Speed'
			})
		]
	});
    this.model = model;
}

util.inherits(SpeedCharacteristic, BlenoCharacteristic);

SpeedCharacteristic.prototype.onWriteRequest = function (data, offset, withoutResponse, callback) {
	console.log("Received bluetooth data for speed (" + data.length + ")");
	if (offset) {
		callback(this.RESULT_ATTR_NOT_LONG);
	} else if (data.length !== 1) {
		callback(this.RESULT_INVALID_ATTRIBUTE_LENGTH);
	} else {
        var percent = data.readInt8(0);
		console.log("Received Bluetooth: " + percent + "%");
		this.model.setMotorSpeed(percent);
		callback(this.RESULT_SUCCESS);
	}
};

SpeedCharacteristic.prototype.onReadRequest = function(offset, callback) {
    if(offset) {
        callback(this.RESULT_ATTR_NOT_LONG, null);
    } else {
        var m = this.model;
        var data = new Buffer(1);
        console.log("Received read request (Speed). Sending " + m.getMotorSpeed() + "%");
        data.writeInt8(this.model.getMotorSpeed(), 0);
        callback(this.RESULT_SUCCESS, data);
    }
}

module.exports = SpeedCharacteristic;