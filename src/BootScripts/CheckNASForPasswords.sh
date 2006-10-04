#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

Q="SELECT IPaddress FROM PnpQueue WHERE PK_PnpQueue=$1"
IP=$(RunSQL "$Q")

echo "Checking IP $IP for passwords"

# send the message CMD_Pre_Pnp_Script_Done with 109 Data String = parm 182 DEVICEDATA_Password_Required_CONST = 1
/usr/pluto/bin/MessageSend $DCERouter 0 $2 1 822 119 1 224 $1 109 "182|1"
