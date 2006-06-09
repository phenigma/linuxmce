#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

## Get a list of repos
Q="SHOW TABLES LIKE 'psc_%_table'"
R=$(RunSQL "$Q")
Repos=
for TblName in $R ;do
	Rname=$(echo $TblName | cut -d'_' -f2)	
	Repos="${Repos} ${Rname}"
done

## Get a list of tables
Q="SHOW TABLES"
R=$(RunSQL "$Q")
Tables=
for TblName in $R ;do
	Tables="${Tables} $TblName"
done

for Rname in $Repos ;do
	echo $Rname
done
