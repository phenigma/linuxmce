#!/bin/bash

IP="$1"; shift

if [ -n "$IP" -a "$#" -ne 0 ]; then
	if [ "$IP" != "127.0.0.1" ]; then
		ssh -l root -i /usr/pluto/keys/id_dsa_pluto_apache "$IP" "$*" || echo "Failed to contact destination computer (IP: $IP)"
	else
		"$@"
	fi
else
	Msg=""
	[ -z "$IP" ] && Msg="${Msg}Empty IP address, "
	[ "$#" -eq 0 ] && Msg="${Msg}Empty command line, "
	echo "${Msg%, }"
fi
