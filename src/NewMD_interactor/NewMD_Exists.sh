#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

MAC="$1"

Q="
	SELECT PK_Device
	FROM Device
	WHERE MACaddress='$MAC'
"
RunSQL "$Q"
