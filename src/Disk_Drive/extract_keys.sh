#!/bin/bash

pushd /root/.wine/dosdevices
ln -s $1 t:
popd
#export DISPLAY=:0
pushd /usr/pluto/tools/aacskeys
WINEDEBUG=-all wine aacskeys.exe t > /tmp/aacskeys.tmp
popd
rm /root/.wine/dosdevices/t:

DiskKey=`cat /tmp/aacskeys.tmp | grep "Unique Key" | cut -c 31- | tr -d \\\\r\\\\n`
DiskID=`cat /tmp/aacskeys.tmp | grep DiscID | cut -c 31- | tr -d \\\\r\\\\n `

#echo "Key: $DiskKey"
#echo "ID: $DiskID"

Line="$DiskID=DISK|V|$DiskKey"
echo $Line >> /usr/pluto/tools/dumphd/KEYDB.cfg

