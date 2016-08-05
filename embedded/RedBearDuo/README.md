# Remote Lego - Embedded - Control board (RedBear DUO)

![Control board](images/ControlBoard.jpg)

The RedBear duo is the brain of the car models. It contains all the car specific logic and is the communication partner for the remote control. It implements the GATT service to expose the car models features.


## Preparation
In order to flash the software to the RedBear duo you have to install the Arduino IDE, set up the correct RedBear plugin and flash the correct firmware to the DUO.

For a Getting Started guide that covers the installation of Arduino and the RedBear plugin refer to this web page: [https://github.com/redbear/Duo/blob/master/docs/getting_started_with_arduino_ide.md](https://github.com/redbear/Duo/blob/master/docs/getting_started_with_arduino_ide.md)

Please make sure that you select the correct version of the plugin. The version the current code is tested with is mentioned in the LegoControl.ino file on the top.
To install the correct firmware for the selected plugin version just use the Arduino mechanism mentioned in the Getting Started guide to flash the firmware.

Additionally you need to install an stl port for Arduino as the lego car code uses standard types (shared_ptr, vector). There is a zip file containing the Arduino in the repository but you can just follow this guide to download and install the stl port:
[https://github.com/vancegroup/stlport-avr/wiki](https://github.com/vancegroup/stlport-avr/wiki).

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
The behavior and the special characteristics the car has are represented by its profile.
The profile has to be set once per car. This can be done by sending the configuration data via two special Bluetooth characteristics.
Currently the Android App is able to handle setting the car up. So the easiest way of making the initial configuration for your car is to power it up, connect the Android app to it and hit the settings button. There you can choose each parameter for the configuration (like steering offset, device name, supported features, ...) and send the configuration to the RedBear DUO.
The configuration then gets persisted in the EEPROM for the DUO so the data has only to be sent once.

