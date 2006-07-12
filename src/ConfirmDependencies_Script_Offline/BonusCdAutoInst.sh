#!/bin/bash

if [[ "$BonusCD" == Y || "$BonusCD" == y ]]; then
	dpkg -i "$BonusWorkDir"/*.deb
	rm -rf "$BonusWorkDir"
fi
