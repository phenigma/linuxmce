#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

echo "Starting ratpoison"

export DISPLAY=:0
screen -d -m -S RatPoison /usr/pluto/bin/ratpoison
sleep 1

