#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

echo "Starting ratpoison"

DISPLAY=:0 /usr/pluto/bin/ratpoison &
sleep 1

