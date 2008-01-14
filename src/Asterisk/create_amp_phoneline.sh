#!/bin/bash

Keyword="$1"
shift

AmpScript="/usr/pluto/bin/create_amp_$Keyword.pl"
"$AmpScript" "$@"
/usr/pluto/bin/create_pluto_dialplan.pl
