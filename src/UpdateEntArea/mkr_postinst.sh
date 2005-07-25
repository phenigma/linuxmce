#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

/usr/bin/mysql pluto_main < /usr/pluto/bin/PurgeEntAreas.sql
/usr/pluto/bin/UpdateEntArea
echo 'DELETE FROM CachedScreens;' | mysql pluto_main || /bin/true
echo 'UPDATE Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory SET NeedConfigure=1 WHERE FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5;' | mysql pluto_main || /bin/true
echo 'UPDATE Orbiter set Modification_LastGen=0,Regen=1;' | mysql pluto_main || /bin/true
