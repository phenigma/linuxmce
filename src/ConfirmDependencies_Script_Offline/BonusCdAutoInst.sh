#!/bin/bash

if [[ "$BonusCD" == Y || "$BonusCD" == y ]]; then
	echo "Installing packages copied earlier from \"Pluto Bonus CD 1\""
	dpkg -i "$BonusWorkDir"/*.deb
	rm -rf "$BonusWorkDir"
fi
