#!/bin/bash

mount /dev/cdrom /media/cdrom || :

if [[ "$(aptitude show nvidia-glx | grep State | cut -d ' ' -f2)" == "installed" ]] ;then
        nvidia_glx_installed="true"
fi

if [[ "$nvidia_glx_installed" == "false" ]] ;then
        dpkg -i /media/cdrom/cachecd1-cache/nvidia-glx_*.deb && nvidia_glx_installed="true"
fi

if [[ "$nvidia_glx_installed" == "false" ]] ;then
        apt-get update
        apt-get -f -y install nvidia-glx && nvidia_glx_installed="true"
fi

if [[ "$(aptitude show linux-restricted-modules-generic | grep State | cut -d ' ' -f2)" == "installed" ]] ;then
        linux_restricted_modules_generic_installed="true"
fi

if [[ "$linux_restricted_modules_generic_installed" == "false" ]] ;then
        apt-get update
        apt-get -f -y install linux-restricted-modules-generic && linux_restricted_modules_generic_installed="true"
fi

#if [[ "$linux_restricted_modules_generic_installed" == "false" ]] ;then
#       dpkg -i 
#fi

if [[ "$linux_restricted_modules_generic_installed" == "false" || "$nvidia_glx_installed" == "false" ]] ;then
        echo "ERROR: Some of the pacakges could not be installed"
        exit 1
fi

