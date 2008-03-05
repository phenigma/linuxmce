#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

if [[ "$DVDKeysCache" == "" ]] ;then
	ConfSet "DVDKeysCache" "/home/.dvdcss"
fi
