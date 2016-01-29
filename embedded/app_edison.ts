import pa = require('./peripherals/edison/EdisonPeripheralAccess');
import mod = require('./model/CarModel');
import bt = require('./bluetooth/BluetoothHandler')

var peripheralAccess = new pa.EdisonPeripheralAccess();
var carModel: mod.CarModel = new mod.CarModel(
    peripheralAccess,
    12, //motor speed pin (pwm)
    15, //motor direction
    14, //motor enable
    13, //servo control pwm
    40000, //motor pwm frequency
    44, //led front
    45, //led back
    46, //led state red
    47, //led state green
    48 //led state blue
);

carModel.setSteering(0);
carModel.setMotorSpeed(0);

var btHandler: bt.BluetoothHandler = new bt.BluetoothHandler(carModel, carModel);