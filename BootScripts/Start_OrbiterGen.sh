. /usr/pluto/bin/pluto.func

SkinDir=/usr/pluto/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto

Q="SELECT PK_Installation FROM Installation LIMIT 1"
installation=$(echo "$Q;" | /usr/bin/mysql pluto_main | tail +2)

Q="SELECT PK_Device FROM Device 
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
LEFT JOIN Orbiter ON PK_Device=PK_Orbiter
WHERE (FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5)
AND (Regen IS Null Or Regen=1)
AND FK_Installation=$installation"

Orbiters=$(echo "$Q;" | /usr/bin/mysql pluto_main | tail +2 | tr '\n' ' ')

export SDL_VIDEODEVICE=dummy

for OrbiterDev in $Orbiters; do
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating Orbiter nr. $OrbiterDev"
	/usr/pluto/bin/OrbiterGen -d "$OrbiterDev" -g "$SkinDir" -f "$FontDir" -o "$OutDir" -h localhost || Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to generate Orbiter nr. $OrbiterDev"
done
