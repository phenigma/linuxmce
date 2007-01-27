#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Device=$2

Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Device'"
R=$(RunSQL "$Q")
for i in $R; do
	/usr/pluto/bin/MessageSend dcerouter -targetType template 0 27 1 246 2 $i 52 152 5 "10102AB8"
	# hardcode the id until this is fixed
done

