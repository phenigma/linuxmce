#!/bin/bash

bytesLeft=`apt-get dist-upgrade < /dev/null | grep '^Need to get' | cut -f4 -d' ' | cut -f1 -d'/'`
if [ "$bytesLeft" == "" ] ;then
    echo "No updates available."
    exit 1
elif [ "$bytesLeft" != "0B" ] ;then
    echo "Updates available. Need to download $bytesLeft."
    exit 1
else
    echo "Updates available."
fi