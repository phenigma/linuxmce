#!/bin/bash
# vim:set ft=sh:

set -e 

. ./filesd-parms.sh $@

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Section_Ops.sh

## Config for nis to act as a client
File="/etc/event.d/pluto"
Content="
start on runlevel-2

stop on shutdown
stop on runlevel-3
stop on runlevel-4
stop on runlevel-5

script
/usr/pluto/bin/Startup_DisklessMD.sh
end script
"

mkdir -p "${Parm_RootLocation}/$(dirname $File)"
echo "${Content}" > "${Parm_RootLocation}/${File}"
