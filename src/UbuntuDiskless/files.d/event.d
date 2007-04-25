#!/bin/bash
# vim:set ft=sh:

set -e 

. ./filesd-parms.sh $@

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Section_Ops.sh

## Startup sequence
File="/etc/event.d/pluto"
Content="
start on runlevel 2

stop on shutdown
stop on runlevel 3
stop on runlevel 4
stop on runlevel 5

script
/usr/pluto/bin/Startup_DisklessMD.sh
end script
"

mkdir -p "${Parm_RootLocation}/$(dirname $File)"
echo "${Content}" > "${Parm_RootLocation}/${File}"

## Shutdown sequence
File="/etc/event.d/pluto-shutdown"
Content="
start on shutdown

script
/usr/pluto/bin/Report_MachineOff.sh
end script
"

echo "$Content" > "$Parm_RootLocation/$File"
