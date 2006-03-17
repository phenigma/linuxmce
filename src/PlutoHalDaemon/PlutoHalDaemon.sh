#!/bin/bash

## Make sure the had / dbus-1 are started
/etc/init.d/dbus-1 start

## Start our daemon in a screen
screen -d -m -S "PlutoHalDaemon.sh" "/usr/pluto/bin/pluto-hald"
