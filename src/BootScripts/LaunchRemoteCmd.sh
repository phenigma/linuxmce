#!/bin/bash

IP="$1"
Cmd="$2"

Log=/var/log/pluto/remote_command.newlog
echo >>"$Log"
date -R >>"$Log"
echo "IP: $IP ; Cmd: $Cmd" >>"$Log"

if [ -n "$IP" -a "$#" -ne 0 ]; then
	Output="$(ssh -l root -i /usr/pluto/keys/id_dsa_pluto_apache "$IP" "$Cmd; echo \$?; sleep 1; kill \$PPID" > >(tee -a "$Log") 2> >(cat >>"$Log"))"
	Ret="$(echo "$Output" | tail -1)"
	
	[[ $Ret -ne 0 && $Ret -ne 10 ]] && echo "Failed to contact destination computer (IP: $IP)" && exit 1
	Lines=$(echo "$Output" | wc -l | awk '{print $1}')
	Output="$(echo "$Output" | awk 'NR < '$Lines' {print}')"
	echo "$Output"
else
	Msg=""
	[ -z "$IP" ] && Msg="${Msg}Empty IP address, "
	[ "$#" -eq 0 ] && Msg="${Msg}Empty command line, "
	echo "${Msg%, }"
fi
