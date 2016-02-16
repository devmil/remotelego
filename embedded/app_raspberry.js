var pa = require('./peripherals/raspberry/RaspPeripheralAccess');
var mod = require('./model/CarModel');
var bt = require('./bluetooth/BluetoothHandler');
var peripheralAccess = new pa.RaspPeripheralAccess();
var carModel = new mod.CarModel(peripheralAccess, 13, //motor speed pin (pwm)
5, //motor direction
6, //motor enable
18, //servo control pwm
40000, //motor pwm frequency
19, //led front
26, //led back
16, //led state red
20, //led state green
21, //led state blue
9, //led front fog
11 //led back drive
);
carModel.setSteering(0);
carModel.setMotorSpeed(0);
var btHandler = new bt.BluetoothHandler("LegoTruck Raspberry", carModel, carModel);
