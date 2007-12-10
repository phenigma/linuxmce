#!/bin/bash

set -e

Distro="$(lsb_release -c -s)"
Arch="$(apt-config dump | grep '^APT::Architecture' | sed  's/.* "\(.*\)";$/\1/g')"

echo "Installing Build System : $Distro-$Arch"

rm -f "/etc/lmce-build"
ln -s "$(pwd)/conf-files/${Distro}-${Arch}/" "/etc/lmce-build"


