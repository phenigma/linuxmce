#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

GetListOfRooms()
{
	Q="
		SELECT PK_Room,
			CONCAT(
				CASE
					WHEN FK_DeviceCategory IS NOT NULL THEN '* '
					ELSE ''
				END, 
				Room.Description
			) AS StarDesc
		FROM Room
		LEFT JOIN Device ON FK_Room=PK_Room
		LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=8 OR FK_DeviceCategory IS NULL
		GROUP BY Room.Description
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
