#!/bin/bash

Databases=(pluto_main)

for ((i = 1; i <= "$#"; i++)); do
	Databases=("${Databases[@]}" "${!i}")
done

for DB in "${Databases[@]}"; do
	Q="select PK_Version from Version WHERE PK_Version<>1 ORDER BY date desc, PK_Version limit 1"
	last_version=$(echo "$Q;" | mysql -N "$DB")

	Q="select VersionName from Version WHERE PK_Version=$last_version"
	last_version_name=$(echo "$Q;" | mysql -N "$DB")

	Q1="UPDATE Version SET VersionName='${last_version_name}.$(date +%g%m%d%H%M)',Date='20$(date +%g%m%d)' WHERE PK_Version=1;"
	echo "$Q1" | mysql -N "$DB"
done
