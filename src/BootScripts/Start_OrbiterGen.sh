#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

SkinDir=/usr/pluto/orbiter/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto/orbiter

/usr/pluto/bin/UpdateEntArea -h localhost > >(tee -a /var/log/pluto/updateea.newlog)

Q="SELECT PK_Installation FROM Installation LIMIT 1"
installation=$(echo "$Q;" | /usr/bin/mysql -h $MySqlHost pluto_main | tail +2)

Q="SELECT FK_DeviceCategory FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_Device=$PK_Device"

if [ $(echo "$Q;" | /usr/bin/mysql -h $MySqlHost -N pluto_main) == 7 ]; then

	# Generate for all the Orbiters with no parent (ie stand-alone orbiters)
	Q="SELECT PK_Device FROM Device 
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
LEFT JOIN Orbiter ON PK_Device=PK_Orbiter
WHERE (FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5)
AND (Regen IS Null Or Regen=1 OR PK_Orbiter IS NULL OR Device.NeedConfigure=1)
AND FK_Installation=$installation"

	Orbiters=$(echo "$Q;" | /usr/bin/mysql -h $MySqlHost pluto_main | tail +2 | tr '\n' ' ')

	export SDL_VIDEODEVICE=dummy

	for OrbiterDev in $Orbiters; do
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating stand-alone Orbiter nr. $OrbiterDev"
		/usr/pluto/bin/OrbiterGen -d "$OrbiterDev" -g "$SkinDir" -f "$FontDir" -o "$OutDir" -h "$MySqlHost" > >(tee -a /var/log/pluto/orbitergen.newlog) 2> >(grep -vF "WARNING: You are using the SDL dummy video driver!" >&2) || Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to generate Orbiter nr. $OrbiterDev"
	done
fi

