#!/bin/bash

set -e


Distro="$(lsb_release -c -s)"
Arch="$(apt-config dump | grep '^APT::Architecture' | sed  's/.* "\(.*\)";$/\1/g')"

# Install default config files
echo "Installing Default Configs For $Distro-$Arch"
rm -f "/etc/lmce-build"
ln -s "$(pwd)/conf-files/${Distro}-${Arch}/" "/etc/lmce-build"

# Generate ssh key for builder if !exist
if [[ ! -f /etc/lmce-build/builder.key ]] ;then
	echo "Generating SSH Key for this host : /etc/lmce-build/builder.key"
	ssh-keygen -N '' -C "LinuxMCE Builder $Distro $Arch" -f /etc/lmce-build/builder.key
else
	echo "SSH Key found on this host : /etc/lmce-build/builder.key"
fi

