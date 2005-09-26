#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

#Example:
#  QUERY="SELECT field1, field2 FROM table"
#  RESULT=$(RunSQL "$QUERY")
#  for ROW in $RESULT; do
#      FIELD1=$(Field 1 "$ROW")
#      FIELD2=$(Field 2 "$ROW")
#      echo "Field1: $FIELD1; Field2: $FIELD2"
#  done

#Usage:
#  Var=$(RunSQL "<SQL query>")
#Returns resulting rows in $Var; you can iterate through them using 'for'
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

#Usage:
#  Field=$(Field 1 "<SQL row>")
#Used in a 'for' loop to extract a field value from a row
Field()
{
	local Row FieldNumber
	FieldNumber="$1"; shift
	Row="$*"
	echo "$Row" | cut -d, -f"$FieldNumber" | tr '~' ' '
}

#Usage:
#  UseDB "<DB name>"
#Changes the default database
UseDB()
{
	SQL_DB="$1"
	if [[ -z "$SQL_DB" ]]; then
		SQL_DB="pluto_main"
	fi
}

UseDB "$MySqlDBName"
