#!/bin/bash

PORT="${1:-7890}"
PARAMS="--audiodir /home/public/data/audio"

sed -si "s/CLI_PORT=.*/CLI_PORT=$PORT/g" /etc/default/slimserver
sed -si "s/PARAMS=.*/PARAMS=\"$PARAMS\"/g" /etc/default/slimserver

invoke-rc.d slimserver restart
