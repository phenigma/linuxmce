#!/bin/bash

PnpQueueID=$1
DeviceID=$2

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

## Get the device ip address from the pnp queue
Q="SELECT IPaddress FROM PnpQueue WHERE PK_PnpQueue=$PnpQueueID"
IPaddress=$(RunSQL "$Q")
if [[ $IPaddress == "" ]] ;then
	echo "Error: Can't find any PnpQueue entry with PK_PnpQueue = $PnpQueueID"
	exit 1
fi

echo "Checking IP $IPaddress for passwords"

# send the message CMD_Pre_Pnp_Script_Done with 109 Data String = parm 182 DEVICEDATA_Password_Required_CONST = 1
/usr/pluto/bin/MessageSend $DCERouter 0 $DeviceID 1 822 119 1 224 $PnpQueueID 109 "182|0"
