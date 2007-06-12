#!/bin/bash -e

UPDATES_XML_URI="http://10.0.0.83/updates.xml"
UPDATES_DIR="/home/updates"

XML_OLD="${UPDATES_DIR}/updates.xml.old"
XML_CURRENT="${UPDATES_DIR}/updates.xml"
XML_NEW="${UPDATES_DIR}/updates.xml.new"
XML_LOCK="${UPDATES_DIR}/updates.xml.lock"

mkdir -p "$UPDATES_DIR"

function Debug {
	echo "$(date -R) $*"
}

## Lock updates.xml
if [[ -f "$XML_LOCK" ]] ;then
	Debug "Cannot perform update, updates.xml is locked by another process"
	exit 1
fi
touch "$XML_LOCK"
trap "rm -f $XML_LOCK" EXIT

## Download new xml file
if ! wget -c --tries=10 --timeout=5 "${UPDATES_XML_URI}" -O "${XML_NEW}" ;then
	Debug "Failed to download new update.xml file"
	exit 1
fi
mv "${XML_NEW}" "${XML_CURRENT}"

## Try to see if there are any differences between the current and last processed xml
touch "${XML_OLD}"
if diff "${XML_CURRENT}" "${XML_OLD}" >/dev/null ;then
	Debug "No differences since last update"
	exit 0
fi

## TODO: Call eugen's application to process the xml and get an status
/usr/pluto/bin/LMCEUpdate || exit 1

## TODO: Notify user that he can update it's system
/usr/pluto/bin/LMCEUpdate_Notify.sh || exit 1

## Update the last processed xml
cp -r "${XML_CURRENT}" "${XML_OLD}"
