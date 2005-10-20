#!/bin/bash

# check if a filesystem is getting full

. /usr/pluto/bin/pluto.func || exit
module=$(basename $0)

Logging $TYPE $SEVERITY_STAGE $module "Entering $module"

filesystems="/"
fssizewarn=90  # Filesystem size warning in percent
mkdir -p /usr/pluto/locks
lock=/usr/pluto/locks/checkDiskSpace.lock

if [ -f $lock ]; then
	Logging $TYPE $SEVERITY_WARNING $module "Lock file ($lock) already exists"
	exit
fi

Logging $TYPE $SEVERITY_NORMAL $module "Locking $module"
touch $lock
Logging $TYPE $SEVERITY_NORMAL $module "Checking for full filesystem..."
for filesystem in $filesystems; do
	df=$(/bin/df --print $filesystem| grep -v '^Filesystem')
	filesystem=$(echo $df|awk '{print $1}')
	type=$(echo $df|awk '{print $2}')
	blocks=$(echo $df|awk '{print $3}')
	used=$(echo $df|awk '{print $4}')
	available=$(echo $df|awk '{print $5}')
	use=$(echo $df|awk '{print $6}'|cut -d"%" -f1)
	mounted=$(echo $df|awk '{print $7}')
	if [ $use -gt $fssizewarn ]; then
		Logging $TYPE $SEVERITY_CRITICAL $module "Filesystem exceeds ${fssizewarn} percent"
	fi
done
Logging $TYPE $SEVERITY_NORMAL $module "Unlocking $module"
rm $lock
