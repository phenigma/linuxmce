#!/bin/bash

state="$1"
IP="$2"

/usr/pluto/bin/LaunchRemoteCmd.sh "$IP" "dpkg --get-selections | grep -F '$state' | cut -f1"
