# Remote Lego - Embedded - Control board (RedBear DUO)

![Control board](images/ControlBoard.jpg)

The RedBear duo is the brain of the car models. It contains all the car specific logic and is the communication partner for the remote control. It implements the GATT service to expose the car models features.


## Preparation
In order to flash the software to the RedBear duo you have to install the Arduino IDE, set up the correct RedBear blugin and flash the correct firmware to the DUO.

For a Getting Started guide that covers the installation of Arduino and the RedBear plugin refer to this web page: [https://github.com/redbear/Duo/blob/master/docs/getting_started_with_arduino_ide.md](https://github.com/redbear/Duo/blob/master/docs/getting_started_with_arduino_ide.md)

Please make sure that you select the correct version of the plugin. The version the current code is tested with is mentioned in the LegoControl.ino file on the top.
To isntall the correct firmware for the selected plugin version just use the Arduino mechanism mentioned in the Getting Started guide to flash the firmware.

## Flash
After you have set up the environment and installed the correct firmware version you should be ready to go. Just hit "Upload" in the Arduino IDE to compile the project and upload it to the RedBear Duo.

## Inner workings
The RedBear Duo code consists of
1. The LegoControl.ino file
This file is only used as an entry point. The system initialization is triggered there and the main cycle gets distributed. All other files are C++ files.
2. AVRCommunication.
This module encapsulates all communication logic with the AVR. It exposes the available commands via CommandData factory methods so that no other module has to know the inner structure of these commands. It also handles the serial protocol used to communicate with the AVR.
3. BLECommunication
This module contains the GATT service configuration and the implementation of the Service and the Characteristics. Here the Link between the Bluetooth communication and the model is made.
4. BlinkControl
This module handles the Blink logic. It controls the central Blink timer and triggers a re-send of the AVR telegrams
5. Bluetooth
This modules creates an abstraction layer on top of the Bluetooth API provided by RedBear. It works with Service and Characteristic classes, handles the advertisement bytes and routes read/write requests to the correct characteristic instance.
6. LegoCarModel
The LegoCarModel is the heart of the car. It contains all the business logic and contains the current car state. Any changes made to the state of the model get translated into according AVR telegrams and get sent to the Mainboard.
7. NanoCommunication
This module encapsulates the communication with the BLW Nano. It basically sends the Eddystone URI to the BLE Nano (or better: ensures that the current URI is transmitted to the BLE Nano). It does so cyclic as the protocol isn't as strict as with the AVRCommunication so that the timeouts are smaller and therefore a immediate transmittion can't be guarantieed. This has been done to not disturb the AVR communication when there is a problem with the BLE Nano communication (e.g. when the module is missing)
8. Profiles
Each car model has its own specifics. These specifics are provided by instances of "ICarProfile". Each car model that we have controlled using this mechanism already has a profile. For the code to compile you have to choose a profile (or create your own).
Some of the specifics are: 
 + maximum steering angle
 - steering offset
 - steering inverted?
 - Available features

