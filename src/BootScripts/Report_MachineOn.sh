#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

Period=60 # announcement period, in seconds

Count=60
while [[ ! -f /usr/pluto/locks/MachineOff ]]; do
	if [[ "$Count" -eq 60 ]]; then
		Count=0
		/usr/pluto/bin/MessageSend "$DCERouter" "$PK_Device" -1000 7 8 # SYSCMD "Machine Alive"
	fi
	sleep 1
	((Count++))
done
