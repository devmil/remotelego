var pa = require('./peripherals/raspberry/RaspPeripheralAccess');
var mod = require('./model/CarModel');
var bt = require('./bluetooth/BluetoothHandler');
var peripheralAccess = new pa.RaspPeripheralAccess();
var carModel = new mod.CarModel(peripheralAccess, 13, 5, 6, 18, true, 40000, -1, -1, -1, -1, -1, -1, -1 //led back drive
);
carModel.setSteering(0);
carModel.setMotorSpeed(0);
var btHandler = new bt.BluetoothHandler("Lego Jeep", carModel, carModel);
