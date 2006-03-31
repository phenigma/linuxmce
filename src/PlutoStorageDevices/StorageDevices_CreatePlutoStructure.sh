#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

DestPath="/test"

Directories="movies,pictures,music,documents,videos"
countDirs=$(echo $Directories | sed 's/,/\n/g' | wc -l)

Q="SELECT PK_Users, UserName FROM Users"
Users=$(RunSQL "$Q")
for i in `seq 1 $countDirs` ;do
	mediaDir=$(echo $Directories | cut -d',' -f$i)
	
	for User in $Users; do
		User_ID=$(Field 1 "$User")
		User_Uname=$(Field 2 "$User")
		
		echo "mkdir $DestPath/user_$User_ID/data/$mediaDir"
	done
done
