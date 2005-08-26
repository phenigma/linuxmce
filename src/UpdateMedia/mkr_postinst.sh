#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
exit
/usr/pluto/bin/UpdateMedia -d /home/public/data
Q="SELECT PK_Users FROM Users"
R="$(RunSQL "$Q")"

for User in $R; do
	Dir="/home/user_$User/data"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir"
	fi
done
