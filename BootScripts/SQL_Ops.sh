#!/bin/bash

RunSQL()
{
	local Q
	Q="$*"
	[ -z "$Q" ] || echo "$Q;" | mysql -N pluto_main | tr '\n\t ' ' ,~'
}

Field()
{
	local Row FieldNumber
	FieldNumber="$1"; shift
	Row="$*"
	echo "$Row" | cut -d, -f"$FieldNumber" | tr '~' ' '
}

