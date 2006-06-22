#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

GetListOfRooms()
{
	Q="
		SELECT PK_Room, Description
		FROM Room
	"
	Rooms=$(RunSQL "$Q")
	Q="
		SELECT COUNT(PK_Room)
		FROM Room
	"
	Count=$(RunSQL "$Q")
	echo "$Count $Rooms"
}

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--rooms) GetListOfRooms ;;
	esac
done
