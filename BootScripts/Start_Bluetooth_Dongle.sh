#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

echo "Starting Bluetooth Dongle"
screen -d -m -S Bluetooth bash -c "(echo \$(date) Starting; /usr/pluto/bin/Bluetooth_Dongle -d 21 -r localhost) | tee Bluetooth.log"
sleep 1

