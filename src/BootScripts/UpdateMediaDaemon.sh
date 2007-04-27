#!/bin/bash

. /usr/pluto/bin/LockUtils.sh

trap 'Unlock "UM" "Update_Media"' EXIT
Lock "UM" "Update_Media"
counter=30
while ((counter > 0)); do
	nice -n 20 /usr/pluto/bin/UpdateMedia -h localhost -d /home/public/data -B -t | tee-pluto -a /var/log/pluto/UpdateMedia.log
	((counter--))
done
