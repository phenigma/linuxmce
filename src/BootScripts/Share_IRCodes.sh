#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

if [[ "$OfflineMode" == "true" ]]; then
	echo "$0: OfflineMode is active, i can't share no IRCodes, skiping"
	exit 0
fi

# Check in dce,ir first since that may change the primary key in InfraredGroup_Command_Preferred
/usr/pluto/bin/sqlCVS -H sqlcvs.plutohome.com -h localhost -a -n -r dce,ir -d "$PK_Users" -U "$PK_Users~nopass" -e checkin >>/var/log/pluto/Share_IR.log
/usr/pluto/bin/sqlCVS -H sqlcvs.plutohome.com -h localhost -a -n -r local -t InfraredGroup_Command_Preferred -d "$PK_Users" -U "$PK_Users~nopass" -e checkin >>/var/log/pluto/Share_IR.log
