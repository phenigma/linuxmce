#!/bin/bash

Tunnel=$(ls /var/run/screen/S-root/*SSH_Tunnel_plutohome)
if [ -z "$Tunnel" ]; then
	/usr/pluto/bin/setup_tunnels.sh
fi
