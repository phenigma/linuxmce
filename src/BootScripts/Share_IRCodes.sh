#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

if [[ "$OfflineMode" == "true" ]]; then
	echo "$0: OfflineMode is active, i can't share no IRCodes, skiping"
	exit 0
fi

# Getting the timestamp to use in description, which should be IR_YYYYMMDDHHmmss_PK_Installation
timestamp=`date +%Y%m%d%H%M%S`

# Check in dce,ir first since that may change the primary key in InfraredGroup_Command_Preferred
/usr/pluto/bin/sqlCVS -H sqlcvs.plutohome.com -h localhost -a -n -r dce,ir -d "IR_${timestamp}_$PK_Installation" -U "$PK_Users~nopass" -e checkin >>/var/log/pluto/Share_IR.log
/usr/pluto/bin/sqlCVS -H sqlcvs.plutohome.com -h localhost -a -n -r local -t InfraredGroup_Command_Preferred -d "IR_${timestamp}_$PK_Installation" -U "$PK_Users~nopass" -e checkin >>/var/log/pluto/Share_IR.log

