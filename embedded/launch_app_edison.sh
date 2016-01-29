#!/bin/bash

# stop all running instances
node_modules/forever/bin/forever stopall
rm -rf ./forever
mkdir forever

#enable bluetooth
rfkill unblock bluetooth
sleep 1s
#stop BlueZ service
systemctl stop bluetooth
sleep 1s
#enable bluetooth device
hciconfig hci0 up
sleep 1s

# Launch the app

#node app_edison.js
node_modules/forever/bin/forever start -o ./forever/app.log -p ./forever app_edison.js
# node_modules/forever/bin/forever start -o ./forever/eddystone.log -p ./forever eddystone.js