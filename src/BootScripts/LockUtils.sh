#!/bin/bash

# TODO: real locks, with PID and locking host inside to detect stale locks

Dir="/usr/pluto/locks"

Lock()
{
	local Lock="$1"

	[ -z "$Lock" ] && return 1
	ln -s "$Dir/$1" "$Dir/$1" 2>/dev/null
}

# there is no ability to check if this our lock yet
Unlock()
{
	local Lock="$1"

	[ -z "$Lock" ] && return 1
	rm -f "$Dir/$1" 2>/dev/null
}

TryLock()
{
	Lock "$1"
}

WaitLock()
{
	until Lock "$1"; do
		sleep 1
	done
}
