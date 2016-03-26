var pa = require('./peripherals/edison/EdisonPeripheralAccess');
var mod = require('./model/CarModel');
var bt = require('./bluetooth/BluetoothHandler');
var peripheralAccess = new pa.EdisonPeripheralAccess();
var carModel = new mod.CarModel(peripheralAccess, 12, 15, 14, 13, false, 40000, 44, 45, 46, 47, 48, -1, -1 //led back drive
);
carModel.setSteering(0);
carModel.setMotorSpeed(0);
var btHandler = new bt.BluetoothHandler("LegoTruck Edison", carModel, carModel);
