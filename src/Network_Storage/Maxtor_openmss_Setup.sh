#!/bin/bash

EnableMaxtorNFS()
{
	local IP="$1" LAN="192.168.80.0/24"
	
	# How this works (because I'm sure I won't understand it later either):
	# purpose: send a list of commands via a free-access telnet server running on the Maxtor Network Storage (1.2.2-openmss2) device
	# 1st part echoes the commands, one per line, ending with "exit"
	# then, it echoes dots
	# this output is sent to socat
	# socat's output is send through another while loop
	# the last while loop exits when it reads "exit", which means it closes socat's stdout
	# socat ends because of no stdout
	# the dot displaying thingy exits because socat exited
	# the dots don't get to the telnet server, but they serve for the ending part, where the "no stdout" condition needs to be triggered
	
	(echo "mkdir -p /shares/mss-hdd/Pluto
		portmap
		rpc.mountd
		rpc.nfsd
		exportfs -io rw,no_root_squash,no_all_squash $LAN:/shares/mss-hdd/Pluto
		exit"
	while :; do echo -n "."; done) | socat STDIO TCP4:"$IP":23 | while read line; do [[ "$line" == *exit* ]] && break; done
}
