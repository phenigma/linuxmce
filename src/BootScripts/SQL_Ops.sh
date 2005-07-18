#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

RunSQL()
{
	local Q Pass
	Q="$*"
	Pass=
	if [[ -n "$MySqlPassword" ]]; then
		Pass="-p$MySqlPassword"
	fi
	if [[ -n "$Q" ]]; then
		echo "$Q;" | mysql -A -N "$SQL_DB" -h "$MySqlHost" -u "$MySqlUser" $Pass | tr '\n\t ' ' ,~' | sed 's/ *$//'
	fi
}

Field()
{
	local Row FieldNumber
	FieldNumber="$1"; shift
	Row="$*"
	echo "$Row" | cut -d, -f"$FieldNumber" | tr '~' ' '
}

UseDB()
{
	SQL_DB="$1"
	if [[ -z "$SQL_DB" ]]; then
		SQL_DB="pluto_main"
	fi
}

UseDB "$MySqlDBName"
