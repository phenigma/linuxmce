#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

SkinDir=/usr/pluto/orbiter/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto/orbiter

/usr/pluto/bin/UpdateEntArea -h localhost > >(tee -a /var/log/pluto/updateea.newlog)

export SDL_VIDEODEVICE=dummy

if [ "$1" = "0" ]; then
	Q="SELECT Device.PK_Device FROM Device
	JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
	JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
	WHERE (FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5)
	AND Device.FK_Installation=$PK_Installation"
else
	Q="SELECT Device.PK_Device FROM Device
	WHERE PK_Device=$1"
fi

Orbiters=$(echo "$Q;" | /usr/bin/mysql -h $MySqlHost pluto_main | tail +2 | tr '\n' ' ')

export SDL_VIDEODEVICE=dummy

for OrbiterDev in $Orbiters; do
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating Orbiter on the fly nr. $OrbiterDev"
	echo "on the fly regen of $OrbiterDev $2 $3" >> /var/log/pluto/orbitergen.newlog
	/usr/pluto/bin/OrbiterGen -d "$OrbiterDev" $3 -g "$SkinDir" -f "$FontDir" -o "$OutDir" -h "$MySqlHost"  >> /var/log/pluto/orbitergen.newlog || Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to generate Orbiter nr. $OrbiterDev"

	# Notify the Orbiter Plugin that we finished
	/usr/pluto/bin/MessageSend localhost 0 $2 1 267 2 $OrbiterDev
done
