#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

RunSQL()
{
	local Q Pass
	Q="$*"
	[ -n "$MySqlPassword" ] && Pass="-p$MySqlPassword"
	[ -z "$Q" ] || echo "$Q;" | mysql -N pluto_main -h $MySqlHost -u $MySqlUser $Pass | tr '\n\t ' ' ,~' | sed 's/ *$//'
}

Field()
{
	local Row FieldNumber
	FieldNumber="$1"; shift
	Row="$*"
	echo "$Row" | cut -d, -f"$FieldNumber" | tr '~' ' '
}

