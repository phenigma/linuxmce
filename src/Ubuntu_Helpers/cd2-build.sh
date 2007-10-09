#!/bin/bash

## Decide what name to use for the iso
iso_name="$1"
if [[ "$iso_name" == "" ]] ;then
	iso_name="linuxmce-1.1"
fi

## Remove old iso
rm -f "/var/www/${iso_name}-cache.iso"

## Build our new iso
pushd /var/plutobuild
	mkisofs -f -J -r -o "${iso_name}-cache.iso" ./cd2-packages
	mv "${iso_name}-cache.iso" /var/www/
popd
