#!/bin/bash

echo 'DELETE FROM CachedScreens;' | mysql pluto_main || /bin/true
echo 'UPDATE Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory SET NeedConfigure=1 WHERE FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5;' | mysql pluto_main || /bin/true
echo 'UPDATE Orbiter set Modification_LastGen=0,Regen=1;' | mysql pluto_main || /bin/true

# Disable OrbiterGen for m/d's since as of .28 it's done on the core
echo 'UPDATE Device_StartupScript JOIN Device on FK_Device=PK_Device JOIN DeviceTemplate on FK_DeviceTemplate=PK_DeviceTemplate SET Enabled=0 WHERE FK_DeviceCategory=8 AND FK_Device_ControlledVia IS NULL AND FK_StartupScript=27' | mysql pluto_main || /bin/true

