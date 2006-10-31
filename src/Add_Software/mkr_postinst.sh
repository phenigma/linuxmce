#!/bin/bash

if [[ -z "$2" ]]; then # 1st install
	/usr/pluto/bin/getxmls || :
fi
