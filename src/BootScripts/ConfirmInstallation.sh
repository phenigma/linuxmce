#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Q="SELECT COUNT(*) FROM Installation WHERE PK_Installation=1"
R=$(RunSQL "$Q")
if [[ "$R" -ne 0 ]]; then
	NewInstNumber=$(wget -O - http://plutohome.com/new_installation.php)
	if [[ -n "$NewInstNumber" ]]; then
		/usr/pluto/bin/sqlCVS -t Installation -O 1 -N "$NewInstNumber" change_key
		ConfSet PK_Installation "$NewInstNumber"
	fi
fi
