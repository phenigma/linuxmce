# hardcoded
SkinDir=/usr/pluto/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto

Q="SELECT PK_Device FROM Device 
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
LEFT JOIN Orbiter ON PK_Device=PK_Orbiter
WHERE (FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5)
AND (Regen IS Null Or Regen=1)
AND FK_Installation=1"

Orbiters=$(echo "$Q;" | /usr/bin/mysql pluto_main | tail +2 | tr '\n' ' ')

for OrbiterDev in $Orbiters; do
	export SDL_VIDEODRIVER=dummy
	/usr/pluto/bin/OrbiterGen -d "$OrbiterDev" -g "$SkinDir" -f "$FontDir" -o "$OutDir" -h localhost
done
