#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/pluto.func

DEVICECATEGORY_Network_Storage=150

Q="SELECT PK_Device,D.Description,DT.Description
	FROM Device D
	JOIN DeviceTemplate DT ON PK_DeviceTemplate=FK_DeviceTemplate
	JOIN DeviceCategory ON PK_DeviceCategory=FK_DeviceCategory
	WHERE PK_DeviceCategory=$DEVICECATEGORY_Network_Storage"
R=$(RunSQL "$Q")

for Device in $R; do
	DevPK=$(Field 1 "$Device")
	DevDesc=$(Field 2 "$Device")
	DevTDesc=$(Field 3 "$Device")

	Logging "Net_Mount_All" $SEVERITY_NORMAL "NAS" "Mounting device '$DevDesc' (PK $DevPK, template: '$DevTDesc')"
	/usr/pluto/bin/Net_Mount.sh -d "$DevPK"
done
