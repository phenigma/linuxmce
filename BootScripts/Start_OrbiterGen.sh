#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

SkinDir=/usr/pluto/orbiter/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto/orbiter

Q="SELECT PK_Installation FROM Installation LIMIT 1"
installation=$(echo "$Q;" | /usr/bin/mysql pluto_main | tail +2)

Q="SELECT FK_DeviceCategory FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_Device=$PK_Device"

if [ $(echo "$Q;" | /usr/bin/mysql -N pluto_main) == 7 ]; then

# Generate for all the Orbiters with no parent (ie stand-alone orbiters)
Q="SELECT PK_Device FROM Device 
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
LEFT JOIN Orbiter ON PK_Device=PK_Orbiter
WHERE (FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5)
AND (Regen IS Null Or Regen=1)
AND FK_Device_ControlledVia IS NULL
AND FK_Installation=$installation"

Orbiters=$(echo "$Q;" | /usr/bin/mysql pluto_main | tail +2 | tr '\n' ' ')

export SDL_VIDEODEVICE=dummy

for OrbiterDev in $Orbiters; do
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating stand-alone Orbiter nr. $OrbiterDev"
	/usr/pluto/bin/OrbiterGen -d "$OrbiterDev" -g "$SkinDir" -f "$FontDir" -o "$OutDir" -h localhost || Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to generate Orbiter nr. $OrbiterDev"
done

fi

# Generate for any orbiters that are running on this system

Q="SELECT Device.PK_Device FROM Device
JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
LEFT JOIN Orbiter ON Device.PK_Device=PK_Orbiter
LEFT JOIN Device As Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia
WHERE (FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5)
AND (Regen IS Null Or Regen=1)
AND (Device.FK_Device_ControlledVia=$PK_Device OR Parent.FK_Device_ControlledVia=$PK_Device)
AND Device.FK_Installation=$installation"

Orbiters=$(echo "$Q;" | /usr/bin/mysql pluto_main | tail +2 | tr '\n' ' ')

export SDL_VIDEODEVICE=dummy

for OrbiterDev in $Orbiters; do
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating our on-screen Orbiter nr. $OrbiterDev"
	/usr/pluto/bin/OrbiterGen -d "$OrbiterDev" -g "$SkinDir" -f "$FontDir" -o "$OutDir" -h localhost || Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to generate Orbiter nr. $OrbiterDev"
done

