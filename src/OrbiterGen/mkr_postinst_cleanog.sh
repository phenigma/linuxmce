#!/bin/bash

echo 'DELETE FROM CachedScreens;' | mysql pluto_main || /bin/true
echo 'UPDATE Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory SET NeedConfigure=1 WHERE FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5;' | mysql pluto_main || /bin/true
echo 'UPDATE Orbiter set Modification_LastGen=0;' | mysql pluto_main || /bin/true
echo 'UPDATE Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate SET NeedConfigure=1 WHERE FK_DeviceCategory IN (2,3,5);' | mysql pluto_main || /bin/true
