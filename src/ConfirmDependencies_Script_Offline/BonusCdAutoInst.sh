#!/bin/bash

if [[ "$BonusCD" != N && "$BonusCD" != n ]]; then
	echo "Installing packages copied earlier from \"Pluto Bonus CD 1\""
	dpkg -i "$BonusAutoInstDir"/*.deb
	rm -rf "$BonusAutoInstDir"
fi
