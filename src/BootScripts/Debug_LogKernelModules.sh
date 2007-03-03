#!/bin/bash

if [[ -n "$1" ]]; then
	Script="$(basename "$1")"
fi

mkdir -p /var/log/pluto
ls -la /lib/modules/*/misc/ &>/var/log/pluto/kernmod."$Script".postinst."$(date -R)".log
