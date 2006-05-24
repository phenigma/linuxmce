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
wget -O "$OutputFile" "$URL"

Header=$(head -1 "$OutputFile")
Footer=$(tail -1 "$OutputFile")

if [[ "$Header" != "-- Database import" || "$Footer" != "-- EOF" ]]; then
	echo "ERROR. Header: $Header; Footer: $Footer"
	exit 1
fi

PSC=$(head -2 "$OutputFile" | tail -1)
PSC="${PSC#-- psc-id: }"

if [[ -n "$Table" && -n "$PSC" ]]; then
	Q="DELETE FROM $Table WHERE psc_id IN ($PSC)"
	RunSQL "$Q"
fi

QPass=
if [[ -n "$MySqlPassword" ]]; then
	QPass="-p$MySqlPassword"
fi
mysql -u $MySqlUser -h $MySqlHost $QPass "$MySqlDBName" <"$OutputFile"
