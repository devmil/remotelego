import pa = require('./peripherals/raspberry/RaspPeripheralAccess');
import mod = require('./model/CarModel');
import bt = require('./bluetooth/BluetoothHandler')

var peripheralAccess = new pa.RaspPeripheralAccess();
var carModel: mod.CarModel = new mod.CarModel(
    peripheralAccess,
    13, //motor speed pin (pwm)
    5, //motor direction
    6, //motor enable
    18, //servo control pwm
    true, //invert servo direction
    40000, //motor pwm frequency
    -1, //led front
    -1, //led back
    -1, //led state red
    -1, //led state green
    -1, //led state blue
    -1,  //led front fog
    -1  //led back drive
);

carModel.setSteering(0);
carModel.setMotorSpeed(0);

var btHandler: bt.BluetoothHandler = new bt.BluetoothHandler("Lego Jeep", carModel, carModel);