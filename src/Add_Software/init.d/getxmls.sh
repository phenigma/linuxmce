#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

if [[ "$OfflineMode" == "true" ]]; then
	exit 0
fi

/usr/pluto/bin/getxmls && rm -f /etc/init.d/getxmls.sh
