#!/bin/bash

#XClient=/usr/bin/icewm-session
XClient=/usr/bin/xfwm4

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--client) ((i++)); XClient=${!i} ;;
	esac
done

killall "$XClient"
