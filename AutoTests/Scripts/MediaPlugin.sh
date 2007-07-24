#!/bin/bash

if [ -e /tmp/MediaPlugin.tmp ] ;then
	rm -f /tmp/MediaPlugin.tmp
fi

#flag='0'
echo >> /MediaPlugin.log
echo "*MEDIA Plugin*" >> /MediaPlugin.log
echo "==============" >> /MediaPlugin.log
echo "MediaPlugin.sh" >> /tmp/status.log

echo >> /MediaPlugin.log

if [ -f "/var/log/pluto/DCERouter.log" ]; then
	nr=$(cat /var/log/pluto/DCERouter.log | grep -F "Media_Plugin::StartMedia" | wc -l)
	echo "---Media_Plugin::StartMedia [$nr]" >> /MediaPlugin.log
else
	echo "This is a MD, no \"StartMedia\" statistics" >> /MediaPlugin.log
fi 

echo >> /MediaPlugin.log


