# Remote Lego - Embedded
This subtree contains all the source code needed for the various embedded controllers that are part of the (Lego) remote controlled car.

##[Mainboard (Atmega)](Atmega/README.md)
The mainboard contains an Atmega microcontroller that is responsible for all the low level hardware communication. It configures timers to generate PWM signals, sets Pins for switching LEDs on and off and has some additional mechanisms to ensure that the car isn't driving away when the connection is lost or the control board hangs up :)
It gets controlled by the RedBear Duo (or any other controller you might use) via serial commands.

##[RedBear Duo](RedBearDuo/README.md)
The Duo is the communication partner for all remote controls. It provides the Bluetooth service needed for communication with the model and it contains all the car specific information (in form of profiles) like maximum steering angle, supported features, steering offset, ...

##[BLE Nano](BLENano/README.md)
This little helper has only one task: Broadcast the Eddystone advertising package. The software for it is designed in a way that it is generic and receives the URI to broadcast via serial commands.