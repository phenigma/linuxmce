#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

SkinDir=/usr/pluto/orbiter/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto/orbiter

export SDL_VIDEODEVICE=dummy

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating Orbiter on the fly nr. $1"
echo "on the fly regen of $1 $2 $3" >> /var/log/pluto/orbitergen.newlog
/usr/pluto/bin/OrbiterGen -d "$1" $3 -g "$SkinDir" -f "$FontDir" -o "$OutDir" -h "$MySqlHost"  >> /var/log/pluto/orbitergen.newlog || Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to generate Orbiter nr. $1"

# Notify the Orbiter Plugin that we finished
/usr/pluto/bin/MessageSend localhost 0 $2 1 267 2 578

