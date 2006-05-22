#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Maxtor_openmss_EnableNFS.sh

DEVICECATEGORY_Network_Storage=158
DEVICETEMPLATE_Maxtor_NAS=1770

Q="SELECT D.PK_Device,D.Description,D.FK_DeviceTemplate,DT.Description,D.IPaddress
	FROM Device D
	JOIN DeviceTemplate DT ON PK_DeviceTemplate=FK_DeviceTemplate
	JOIN DeviceCategory ON PK_DeviceCategory=FK_DeviceCategory
	WHERE PK_DeviceCategory=$DEVICECATEGORY_Network_Storage"
R=$(RunSQL "$Q")

for Device in $R; do
	DevPK=$(Field 1 "$Device")
	DevDesc=$(Field 2 "$Device")
	DevTPK=$(Field 3 "$Device")
	DevTDesc=$(Field 4 "$Device")
	DevIP=$(Field 5 "$Device")

	if [[ "$DevTPK" == "$DEVICETEMPLATE_Maxtor_NAS" ]]; then
		EnableMaxtorNFS "$DevIP"
	fi

	Logging "Net_Mount_All" $SEVERITY_NORMAL "NAS" "Mounting device '$DevDesc' (PK $DevPK, template: '$DevTDesc')"
	/usr/pluto/bin/Net_Mount.sh -d "$DevPK"
done
