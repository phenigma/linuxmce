#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

touch /usr/pluto/locks/MachineOff
sleep 1 # sync with "Machine Online" reporter
/usr/pluto/bin/MessageSend "$DCERouter" "$PK_Device" -1000 7 9 # SYSCMD "Machine Offline"
