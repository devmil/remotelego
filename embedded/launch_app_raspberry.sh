#!/bin/bash

# stop all running instances
node_modules/forever/bin/forever stopall
rm -rf ./forever
mkdir forever

#stop BlueZ service
service bluetooth stop
sleep 1s
#enable bluetooth device
hciconfig hci0 up
hciconfig hci1 up
sleep 1s

# Launch the app

#BLENO_HCI_DEVICE_ID=1 node app_raspberry.js
BLENO_HCI_DEVICE_ID=1 node_modules/forever/bin/forever start -o ./forever/app.log -p ./forever app.js
BLENO_HCI_DEVICE_ID=0 node_modules/forever/bin/forever start -o ./forever/eddystone.log -p ./forever eddystone.js