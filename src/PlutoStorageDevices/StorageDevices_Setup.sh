#!/bin/bash

## Start autmounter
if [[ -x /etc/init.d/autofs ]]; then
	/etc/init.d/autofs restart
else
	echo "WARNING: Autmount daemon doesn't exist ?!"
fi

## Setup mount for all storage devices (runs on every computer)
/usr/pluto/bin/StorageDevices_Mounts.sh

## Exports our local storage devices so other computers can access them (runs on every computer)
/usr/pluto/bin/StorageDevices_ExportsNFS.sh

## Creates a Pluto Directory structure on local devices (runs on every computer)
/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh

## Creates symlinks to storage device in /home (runs only on core)
/usr/pluto/bin/StorageDevices_Symlinks.sh

