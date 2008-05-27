#!/bin/sh

if [[ -n "$(ps | grep -q /usr/bin/DCERouter | grep -v grep)" ]]; then
	exit
fi

while [[ ! -f /var/run/pluto.stop ]]; do
	/usr/bin/DCERouter
done
