#!/bin/bash

if [[ -n "$HEADER_SQL_Ops" ]]; then
	return 0
fi
HEADER_SQL_Ops=included

PRIVATE_SQL_PID=$$
PRIVATE_SQL_MySQL_PID=

SQL_PipeDir=/usr/pluto/SQL_pipes

. /usr/pluto/bin/Config_Ops.sh

CSQL_DIR="/var/run/SQL_Ops"
CSQL_ID=""

#Example:
#  QUERY="SELECT field1, field2 FROM table"
#  RESULT=$(RunSQL "$QUERY")
#  for ROW in $RESULT; do
#      FIELD1=$(Field 1 "$ROW")
#      FIELD2=$(Field 2 "$ROW")
#      echo "Field1: $FIELD1; Field2: $FIELD2"
#  done

# NOTE: This script uses file descriptors 8 and 9. Be to leave them free if you don't want trouble.

# Checks if we're the owner of the MySQL connection and resets state variables if not
PRIVATE_SQL_CheckOwner()
{
	if [[ "$PRIVATE_SQL_PID" != $$ ]]; then
		# We're probably in a child process, so we can't use the parent's connection
		# Or we're in a new process, again without a usable connection
		PRIVATE_SQL_PID=$$
		PRIVATE_SQL_MySQL_PID=
	fi

	mkdir -p "$SQL_PipeDir"
	if [[ ! -p "$SQL_PipeDir/Q_$PRIVATE_SQL_PID" ]]; then
		rm -f "$SQL_PipeDir/Q_$PRIVATE_SQL_PID"
		mkfifo "$SQL_PipeDir/Q_$PRIVATE_SQL_PID"
	fi
	if [[ ! -p "$SQL_PipeDir/R_$PRIVATE_SQL_PID" ]]; then
		rm -f "$SQL_PipeDir/R_$PRIVATE_SQL_PID"
		mkfifo "$SQL_PipeDir/R_$PRIVATE_SQL_PID"
	fi
}

#Starts the MySQL client process
SQL_Connect()
{
	PRIVATE_SQL_CheckOwner
	if [[ -n "$PRIVATE_SQL_MySQL_PID" ]]; then
		# already connected
		return 0
	fi

	local Host="$1" User="$2" Password="$3" Database="$4"
	if [[ -z "$Host" ]]; then
		Host="$MySqlHost"
	fi
	if [[ -z "$User" ]]; then
		User="$MySqlUser"
	fi
	if [[ -z "$Password" ]]; then
		Password="$MySqlPassword"
	fi
	if [[ -z "$Database" ]]; then
		Database="$MySqlDBName"
	fi
	local Pass

	Pass=
	if [[ -n "$Password" ]]; then
		Pass="-p$Password"
	fi

	SQL_DB="$Database"

	mysql -fANn "$SQL_DB" -h "$Host" -u "$User" $Pass <"$SQL_PipeDir/Q_$PRIVATE_SQL_PID" >"$SQL_PipeDir/R_$PRIVATE_SQL_PID" &
	PRIVATE_SQL_MySQL_PID=$!
	exec 8>"$SQL_PipeDir/Q_$PRIVATE_SQL_PID" 9<"$SQL_PipeDir/R_$PRIVATE_SQL_PID"
}

#Stops the MySQL client process
SQL_Disconnect()
{
	PRIVATE_SQL_CheckOwner
	if [[ -z "$PRIVATE_SQL_MySQL_PID" ]]; then
		return 0
	fi

	#kill "$PRIVATE_SQL_MySQL_PID"
	exec 8>&- 9<&-
	wait "$PRIVATE_SQL_MySQL_PID"
	PRIVATE_SQL_MySQL_PID=
}

#Usage:
#  Var=$(RunSQL "<SQL query>")
#Returns resulting rows in $Var; you can iterate through them using 'for'
RunSQL()
{
	SQL_Connect
	local Q Row
	Q="$*"
	if [[ -n "$Q" ]]; then
		echo "$Q;"$'\n'"SELECT '--END-OF-QUERY--';" >&8
		while read Row; do
			if [[ "$Row" == '--END-OF-QUERY--' ]]; then
				break
			fi
			echo "$Row"
		done <&9 | SQL_Translate
	fi
}

RunCSQL()
{
	local Q Q_id
	Q="$*"

	if [[ "$CSQL_ID" == "" ]] ;then
		RunSQL "$Q"
		return
	fi

	Q_id=$(echo "$Q" | sha1sum - | cut -d' ' -f1)
	if [[ ! -f "$CSQL_DIR/$CSQL_ID/$Q_id" ]] ;then
		RunSQL "$Q" > "$CSQL_DIR/$CSQL_ID/$Q_id"
	fi

	cat "$CSQL_DIR/$CSQL_ID/$Q_id"
}

PurgeCSQL() {
	CSQL_ID="$1"
	rm -rf "$CSQL_DIR/$CSQL_ID/*"
}

InitCSQL() {
	CSQL_ID="$1"
	mkdir -p "$CSQL_DIR/$CSQL_ID" 
}

#Usage:
#  Field=$(Field 1 "<SQL row>")
#Used in a 'for' loop to extract a field value from a row
Field()
{
	local Row FieldNumber
	FieldNumber="$1"; shift
	Row="$*"
	echo "$Row" | cut -d$'\x01' -f"$FieldNumber" | tr $'\x02' ' '
}

#Usage:
#  UseDB "<DB name>"
#Changes the default database
UseDB()
{
	SQL_DB="$1"
	if [[ -z "$SQL_DB" ]]; then
		SQL_DB="$MySqlDBName"
	fi

	if [[ -n "$PRIVATE_SQL_MySQL_PID" ]]; then
		# update database connection to match change
		RunSQL "USE $SQL_DB"
	fi
	return 0
}

SQL_Translate()
{
	tr '\n\t ' $'\x20'$'\x01'$'\x02' | sed 's/ *$//'
}

SQL_Connect "$MySqlHost" "$MySqlUser" "$MySqlPassword" "$MySqlDBName"
