#!/bin/bash

. /usr/pluto/bin/LockUtils.sh

trap 'Unlock "UM" "Update_Media"' EXIT
Lock "UM" "Update_Media"

dir_list="/home/public/data"
for dir in /home/user_*/;do 
	if [[ -d $dir/data ]] ;then 
		dir_list="${dir_list}||${dir}data" 
	fi
done

counter=30
while ((counter > 0)); do
	nice -n 20 /usr/pluto/bin/UpdateMedia -h localhost -d "${dir_list}" -B -t | tee-pluto -a /var/log/pluto/UpdateMedia.log
	((counter--))
done
