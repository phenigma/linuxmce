#!/bin/bash

NoSpace="s/ //g"

ConfEval()
{
	[ -e /etc/pluto.conf ] || exit 0
	sed "$NoSpace" /etc/pluto.conf >/tmp/pluto.conf
	while read line; do
		eval "export $line" &>/dev/null
	done </tmp/pluto.conf
}

ConfSet()
{
	Variable="$1"
	Value="$2"
	Line="$Variable = $Value"
	if ! grep "$Variable.*=" /etc/pluto.conf &>/dev/null; then
		echo "$Line" >> /etc/pluto.conf
	else
		sed -i "s/^.*$Variable.*=.*$/$Line/g" /etc/pluto.conf
	fi
	eval "export $Variable=\"$Value\""
}

ConfGet()
{
	Variable="$1"
	if ! grep "$Variable.*=" /etc/pluto.conf &>/dev/null; then
		return 1
	else
		Line=$(grep "$Variable.*=" /etc/pluto.conf 2>/dev/null | head -1 | sed "$NoSpace")
		eval "export $Line"
	fi
}

ConfEval
VGcmd=""
if [ -n "$Valgrind" ]; then
	if which valgrind &>/dev/null; then
		VGcmd="valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=15 " # trailing space is mandatory
	else
		echo "*** WARNING *** Valgrind not installed. Running programs the normal way ***"
	fi
fi
ulimit -c unlimited
