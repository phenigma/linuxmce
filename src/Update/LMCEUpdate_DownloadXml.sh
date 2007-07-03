#!/bin/bash -e

. /usr/pluto/bin/SQL_Ops.sh
FK_DeviceTemplate=$(RunSQL "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device='$PK_Device'")
if [[ "$FK_DeviceTemplate" != "7" ]] ;then
	exit 0
fi

if [[ "$1" != "background" ]] ;then
        screen -d -m -S LMCEUpdate_DownloadXML "$0" background
        exit 0
fi


UPDATES_XML_URI="http://updates.linuxmce.com/updates.xml"
UPDATES_DIR="/home/updates"

XML_OLD="${UPDATES_DIR}/updates.xml.old"
XML_CURRENT="${UPDATES_DIR}/updates.xml"
XML_NEW="${UPDATES_DIR}/updates.xml.new"
XML_LOCK="${UPDATES_DIR}/updates.xml.lock"

mkdir -p "$UPDATES_DIR"

function Debug {
	echo "$(date -R) $*"
	echo "$(date -R) LMCEUpdate_DownloadXml.sh $*" >> /var/log/pluto/UpdateHelper.log
}

Debug "DownloadXml started"

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

Debug "Calling LMCEUpdate to parse the xml file"
## TODO: Call eugen's application to process the xml and get an status
if  ! /usr/pluto/bin/LMCEUpdate ;then
	Debug "LMCEUpdate Failed to parse the xml file or to download updates"
fi

## TODO: Notify user that he can update it's system
Debug "Notifying the user that he can apply the updates"
/usr/pluto/bin/LMCEUpdate_Notify.sh || exit 1
