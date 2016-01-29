var util = require('util');

var bleno = require('bleno');
var BlenoPrimaryService = bleno.PrimaryService;

var SpeedCharacteristic = require('./SpeedCharacteristic');
var SteerCharacteristic = require('./SteerCharacteristic');
var bc = require('./BluetoothConstants');

function LegoCarService(model) {
	LegoCarService.super_.call(this, {
		uuid: bc.BluetoothConstants.SERVICE_LEGOCAR_GUID,
		characteristics: [
			new SpeedCharacteristic(model),
            new SteerCharacteristic(model)
		]
	});
}

util.inherits(LegoCarService, BlenoPrimaryService);

module.exports = LegoCarService;