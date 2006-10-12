#!/bin/bash

# This script is used by Pluto Plug&Play mechanism to check if a network storage device needs a password
# so that it can comunicate with us.
#
# Basicaly is called before the user is presented with any configuration options in orbiter.
#
# Note : For now is used only to see if a samba server needs a password so we can browse his shares


Device_PnpQueueID=$1
Device_ID=$2

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh


# Checks is a samba server needs a password so we can browse his shares
# 	@Param 1: IP address of the samba server
function SambaServerNeedsPassword() {
	local IP=$1
	local NeedsPassword="no"

	## This command should return errcode = 0 if a password is required to scan the share list
	smbclient -L 192.168.80.223 --no-pass | grep -q "Error returning browse list: NT_STATUS_ACCESS_DENIED"
	local errCode=$?

	if [[ "$errCode" == "0" ]] ;then
		NeedsPassword="yes"
	fi

	echo $NeedsPassword
}

## Get the info about the device fron PnpQueue table (the device is not created yet ?)
Q="SELECT IPaddress FROM PnpQueue WHERE PK_PnpQueue=$Device_PnpQueueID"
Device_IPaddress=$(RunSQL "$Q")


## Send the message CMD_Pre_Pnp_Script_Done with 109 Data String = parm 182 DEVICEDATA_Password_Required_CONST = 1
if [[ "$(SambaServerNeedsPassword $Device_IPaddress)" == "yes" ]] ;then
	/usr/pluto/bin/MessageSend $DCERouter 0 $DeviceID 1 822 119 1 224 $PnpQueueID 109 "182|1"
else
	/usr/pluto/bin/MessageSend $DCERouter 0 $DeviceID 1 822 119 1 224 $PnpQueueID 109 "182|0"	
fi


