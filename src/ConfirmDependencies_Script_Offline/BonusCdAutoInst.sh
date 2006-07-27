#!/bin/bash

BonusWorkDir="/usr/pluto/deb-cache/dists/sarge/main/binary-i386"

if [[ "$BonusCD" != N && "$BonusCD" != n ]]; then
	echo "Installing packages copied earlier from \"Pluto Bonus CD 1\""
	echo "Processing video-wizard-videos package ..."
	cd $BonusWorkDir
        dpkg -i video-wizard-videos-*.deb 1>/dev/null 2>/dev/null
fi

