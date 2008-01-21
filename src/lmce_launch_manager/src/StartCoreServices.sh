#!/bin/bash

echo "$(date -R) Starting Core Services - config device changes" >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/Config_Device_Changes.sh >> /var/log/pluto/StartCoreServices.log 2>&1

echo "$(date -R) Starting Core Services - start orbitergen " >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/Start_OrbiterGen.sh >> /var/log/pluto/StartCoreServices.log 2>&1

/usr/pluto/bin/UpdateMediaDaemonControl.sh -enable

echo "$(date -R) Starting Core Services - start dcerouter " >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/Start_DCERouter.sh >> /var/log/pluto/StartCoreServices.log 2>&1
echo "$(date -R) Starting Core Services done" >> /var/log/pluto/StartCoreServices.log 2>&1

#wake mds
/usr/pluto/bin/WakeMDs.sh

#chech if there are raids from previous install (will run only once)
/usr/pluto/bin/checkforRaids.sh
