#!/bin/bash

## Rerun dhcp_config on the core (radu's suggestion)
if [[ ! -f /etc/diskless.conf ]] ;then
	/usr/pluto/bin/DHCP_config.sh
fi
