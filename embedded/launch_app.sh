#!/bin/bash

systemctl stop bluetooth
hciconfig hci0 down
hciconfig hci0 up
hciconfig hci1 down
hciconfig hci1 up

# Unexport all gpios
gpioDirRegex='^/sys/class/gpio/gpio([0-9][0-9]?)$'

for d in /sys/class/gpio/*; do 
    if [[ $d =~ $gpioDirRegex ]]; then
        num=${BASH_REMATCH[1]}
        echo $num > /sys/class/gpio/unexport
    fi
done

# Launch the app
node_modules/forever/bin/forever stopall
rm -rf ./forever
mkdir forever
node_modules/forever/bin/forever start -o ./forever/app.log -p ./forever app.js
node_modules/forever/bin/forever start -o ./forever/eddystone.log -p ./forever eddystone.js