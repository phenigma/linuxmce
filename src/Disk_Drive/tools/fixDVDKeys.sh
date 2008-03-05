#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

export DVDCSS_CACHE="$DVDKeysCache"

CheckKeysForDVD() {
    echo -n "Checking keys for $1 ... "
    if [ -f "$1.keys.tar.gz" ]; then
	echo "present"
	return
    fi
    
    echo "not present, trying to retrieve them from DVD image"

    KeysPath=`/usr/pluto/bin/dvdcssHelper.sh -f "$1" | grep "CACHE_DIR:" | sed 's/CACHE_DIR://'`
    if [ -z "$KeysPath" ]; then
	echo -e "Error retrieving keys: cannot find DVDCSS keys dir for this DVD image\n"
	return
    fi

    DumpStatus=`/usr/pluto/bin/dvdcssHelper.sh -d "$1"`
    if [ "$DumpStatus" != "KEY_ERROR_COUNT:0" ]; then
	echo -e "Error retrieving keys: not all keys can be retrieved from DVD image, don't packing them\n"
	return
    fi

    KeysFolder=`basename $KeysPath`
    KeysCacheFolder=`dirname $KeysPath`

    pushd $KeysCacheFolder > /dev/null
    tar zcf "$1.keys.tar.gz.in-progress" $KeysFolder
    popd > /dev/null
    mv "$1.keys.tar.gz.in-progress" "$1.keys.tar.gz"
    echo -e "Keys retrieved OK\n"    
}

if [ ! -f "/usr/lib/libdvdcss.so.2" ]; then
	echo "Error: no libdvdcss installed. Please install it and re-run this script"
	exit 1
fi

CheckKeysForDVD "$1"

