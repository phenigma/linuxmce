#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Q="insert into Device_DeviceData(FK_Device,FK_DeviceData) select PK_Device,150 FROM Device where FK_DeviceTemplate=62"
RunSQL "$Q"

Q="update Device_DeviceData set IK_DeviceData=209 where FK_DeviceData=150"
RunSQL "$Q"

