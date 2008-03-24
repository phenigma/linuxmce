#!/bin/bash

PORT="${1:-7890}"
PARAMS="--cliport $PORT --audiodir /home/public/data/audio"

sed -ri "s,SLIMOPTIONS=.*,SLIMOPTIONS=\"$PARAMS\",g" /etc/default/slimserver

invoke-rc.d slimserver restart
