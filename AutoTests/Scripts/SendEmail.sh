#!/bin/bash

#MAILADDR="devplutohome@gmail.com"
MAILADDR="developers@plutohome.com -c testing@plutohome.com"

echo "SendEmail.sh" >> /tmp/status.log

cat /critical.log /Unhalded.log /DeviceDied.log /PlayedMedia.log /MediaPlugin.log /CoreDumps.log > /allLogs.log
sed 's|[[0-9]*[;]*[0-9]*m||g' /allLogs.log > /allLogsFin.log

ipThis=`ssh dcerouter ifconfig | sed -e 's/.*inet addr:\(10\.0\.1\.[0-9]*\).*/\1/;t;d;'`
ipThis="${ipThis}_$(hostname)"

#if [[ -f /etc/diskless.conf ]] ;then
#    ipThis="${ipThis}_$(ifconfig eth0 | grep 'inet addr' | cut -d':' -f2 | cut -d' ' -f1)"
#fi
    

DATE="$(date -R)"
cat /allLogsFin.log | mail -s "Automatic Testing Statistics for $ipThis - $DATE" $MAILADDR

rm -f /critical.log /Unhalded.log /DeviceDied.log /PlayedMedia.log /MediaPlugin.log /allLogs.log /CoreDumps.log /uncompress.log 
