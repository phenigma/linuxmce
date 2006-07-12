#!/bin/bash

if [[ "$BonusCD" == Y || "$BonusCD" == y ]]; then
	echo "Installing packages copied earlier from \"Pluto Bonus CD 1\""
	dpkg -i "$BonusAutoInstDir"/*.deb
	rm -rf "$BonusAutoInstDir"
fi
