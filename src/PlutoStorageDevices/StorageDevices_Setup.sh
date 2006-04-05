#!/bin/bash

## TrigerCascade="true" result in script being run on the other computers too
Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                --cascade) ((i++)); TrigerCascade="true" ;;
                --nocascade) ((i++)); TrigerCascade="false" ;;
        esac
done

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

## Call this script on the other machines too
if [[ $TrigerCascade == "true" ]] ;then
	/usr/pluto/bin/MessageSend dcerouter -targetType template 0 26 1 67 13 "/usr/pluto/bin/StorageDevices_Setup.sh" 51 "--nocascade"
fi
