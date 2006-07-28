#!/bin/bash

BonusWorkDir="/usr/pluto/deb-cache/dists/sarge/main/binary-i386"

if [[ "$BonusCD" != N && "$BonusCD" != n ]]; then
	echo "Installing packages copied earlier from \"Pluto Bonus CD 1\""
	echo "Processing video-wizard-videos package ..."
	cd $BonusWorkDir
        dpkg -i video-wizard-videos-*.deb 1>/dev/null 2>/dev/null
	# if monster-nucleus package is detected
        if [[ -f $(ls | grep monster-nucleus) && ! -z $(ls | grep monster-nucleus) ]]; then
	        apt-get -f -y install monster-nucleus
        	apt-get -f -y install bootsplash-theme-monster
        fi
fi

