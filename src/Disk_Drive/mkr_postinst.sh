#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

DVDCSS_CACHE="/home/.dvdcss"

if [[ "$DVDKeysCache" == "" ]] ;then
	ConfSet "DVDKeysCache" "$DVDCSS_CACHE"
fi

if [ ! -d "$DVDCSS_CACHE" ] ;then
    mkdir "$DVDCSS_CACHE"
    echo -e -n "Signature: 8a477f597d28d172789f06886806bc55\r\n# This file is a cache directory tag created by libdvdcss.\r\n# For information about cache directory tags, see:\r\n#   http://www.brynosaurus.com/cachedir/\r\n" > "$DVDCSS_CACHE/CACHEDIR.TAG"
fi

