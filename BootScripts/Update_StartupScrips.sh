#!/bin/bash
. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Updating startup scripts"

Q="INSERT INTO Device_StartupScript(FK_Device,FK_StartupScript,Boot_Order,Background,Enabled,Parameter)
SELECT PK_Device, PK_StartupScript,Hybrid_Boot_Order,Hybrid_Background,Hybrid_Enabled,Hybrid_Parameter FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate 
JOIN StartupScript ON 1=1
LEFT JOIN Device_StartupScript ON Device_StartupScript.FK_Device=PK_Device AND Device_StartupScript.FK_StartupScript=PK_StartupScript
WHERE FK_DeviceCategory=8 AND FK_Device_ControlledVia IS NOT NULL
AND Device_StartupScript.FK_Device IS NULL"

echo "$Q;" | /usr/bin/mysql pluto_main

Q="INSERT INTO Device_StartupScript(FK_Device,FK_StartupScript,Boot_Order,Background,Enabled,Parameter)
SELECT PK_Device, PK_StartupScript,MD_Boot_Order,MD_Background,MD_Enabled,MD_Parameter FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate 
JOIN StartupScript ON 1=1
LEFT JOIN Device_StartupScript ON Device_StartupScript.FK_Device=PK_Device AND Device_StartupScript.FK_StartupScript=PK_StartupScript
WHERE FK_DeviceCategory=8 AND FK_Device_ControlledVia IS NULL
AND Device_StartupScript.FK_Device IS NULL"

echo "$Q;" | /usr/bin/mysql pluto_main

Q="INSERT INTO Device_StartupScript(FK_Device,FK_StartupScript,Boot_Order,Background,Enabled,Parameter)
SELECT PK_Device, PK_StartupScript,Core_Boot_Order,Core_Background,Core_Enabled,Core_Parameter FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate 
JOIN StartupScript ON 1=1
LEFT JOIN Device_StartupScript ON Device_StartupScript.FK_Device=PK_Device AND Device_StartupScript.FK_StartupScript=PK_StartupScript
WHERE FK_DeviceCategory=7
AND Device_StartupScript.FK_Device IS NULL"

echo "$Q;" | /usr/bin/mysql pluto_main
