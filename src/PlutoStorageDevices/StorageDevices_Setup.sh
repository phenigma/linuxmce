#!/bin/bash

if [[ -x /etc/init.d/autofs ]]; then
	/etc/init.d/autofs restart
else
	echo "WARNING: Autmount daemon doesn't exist ?!"
fi

/usr/pluto/bin/StorageDevices_ExportsNFS.sh
/usr/pluto/bin/StorageDevices_Mounts.sh
/usr/pluto/bin/StorageDevices_Symlinks.sh

