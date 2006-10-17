#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

URL="$1"
Table="$2"

if [[ -z "$URL" ]]; then
	echo "ERROR. Empty URL"
	exit 1
fi

OutputFile=$(mktemp)
trap "rm -f '$OutputFile'" EXIT
wget --timeout=10 -O "$OutputFile" "$URL"
# test if offline
RET=$?
if [[ "$RET" == "1" ]]; then
        /usr/pluto/bin/MessageSend dcerouter 0 20 1 741 159 53 9 'The device was added but is missing some config data which can be downloaded only from internet. The device will not work properly until that info is downloaded.'
        exit 1
fi

Header=$(head -1 "$OutputFile")
Footer=$(tail -1 "$OutputFile")

if [[ "$Header" != "-- Database import" || "$Footer" != "-- EOF" ]]; then
	echo "ERROR. Header: $Header; Footer: $Footer"
	exit 1
fi

PSC=$(head -2 "$OutputFile" | tail -1)
PSC="${PSC#-- psc_id: }"

if [[ -n "$Table" && -n "$PSC" ]]; then
	## Get a list of local modified records
	Q="SELECT pcd_id FROM $Table WHERE psc_id IN ($PSC) AND psc_mod != 0"
	LocalModifiedPscIDs=$(RunSQL "$Q" | tr ' ' ',')

	## Remove all non local modified records
	Q="DELETE FROM $Table WHERE psc_id IN ($PSC) AND psc_mod=0"

	## Add the updated recoreds that we get from the server
	RunSQL "$Q"

	## Reset the psc_mod of the records that where syncronized
	Q="UPDATE $Table SET psc_mod=0 WHERE psc_id IN ($PSC) AND NOT psc_id IN ($LocalModifiedPscIDs)"
	RunSQL "$Q"
fi

QPass=
if [[ -n "$MySqlPassword" ]]; then
	QPass="-p$MySqlPassword"
fi
mysql -f -u $MySqlUser -h $MySqlHost $QPass "$MySqlDBName" <"$OutputFile"
