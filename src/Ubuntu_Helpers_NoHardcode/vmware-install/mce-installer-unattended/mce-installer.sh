#!/bin/bash

## Copy fake frontend configuration file in /tmp
cp ./mce_wizard_data.sh /tmp

## Run the mce-installer scripts
./mce-installer-InstallDependencies.sh 
./mce-installer-core.sh

## Run any addons
for addon in ./addons/*.sh ;do
	if [[ -x "$addon" ]] ;then
		./"$addon"
	fi
done

## Reboot the system
init 0
