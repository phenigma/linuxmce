#!/bin/bash

IP="$1"
Cmd="$2"

Log=/var/log/pluto/remote_command.newlog
echo >>"$Log"
date -R >>"$Log"
echo "IP: $IP ; Cmd: $Cmd" >>"$Log"

if [ -n "$IP" -a "$#" -ne 0 ]; then
	if [ "$IP" != "127.0.0.1" ]; then
		ssh -l root -i /usr/pluto/keys/id_dsa_pluto_apache "$IP" "$Cmd" > >(tee -a "$Log") 2> >(cat >>"$Log")
		[ $? -ne 0 -a $? -ne 10 ] && echo "Failed to contact destination computer (IP: $IP)"
	else
		eval "$Cmd"
	fi
else
	Msg=""
	[ -z "$IP" ] && Msg="${Msg}Empty IP address, "
	[ "$#" -eq 0 ] && Msg="${Msg}Empty command line, "
	echo "${Msg%, }"
fi
