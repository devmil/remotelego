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

#check eth0
ip="$(ifconfig | grep -A 1 eth0 | tail -1 | cut -d ':' -f 2 | cut -d ' ' -f 1)"

if [ "$ip" == "" ]; then
    ip="$(ifconfig | grep -A 1 wlan0 | tail -1 | cut -d ':' -f 2 | cut -d ' ' -f 1)"
fi

if [ "$ip" == "" ]; then
    echo "No IP found! Didn't start debugging"
else
    node-inspector --web-host $ip app.js
fi

