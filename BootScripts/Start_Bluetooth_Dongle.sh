#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.conf

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting Bluetooth Dongle"
screen -d -m -S Bluetooth bash -c "(echo \$(date) Starting; /usr/pluto/bin/Bluetooth_Dongle -d 21 -r localhost) | tee Bluetooth.log"
sleep 1

