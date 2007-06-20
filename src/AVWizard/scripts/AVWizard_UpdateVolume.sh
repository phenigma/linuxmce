#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

VolumePercent="$1"
/usr/pluto/bin/MessageSend dcerouter -targetType template -bl same_computer $PK_Device 26 1 313 76 "$VolumePercent"
