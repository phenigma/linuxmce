#!/bin/bash

# TODO: real locks, with PID and locking host inside to detect stale locks

Dir="/usr/pluto/locks"

Lock()
{
	local Lock="$1"
	local NoLog="$2"
	[[ -z "$NoLog" ]] && echo -n "$(date) Lock '$1' "

	[ -z "$Lock" ] && return 1
	if ln -s "$Dir/$1" "$Dir/$1" 2>/dev/null; then
		[[ -z "$NoLog" ]] && echo "fail"
	else
		[[ -z "$NoLog" ]] && echo "success"
	fi
}

# there is no ability to check if this our lock yet
Unlock()
{
	echo -n "$(date) Unlock '$1' "
	local Lock="$1"

	[ -z "$Lock" ] && echo 'fail' && return 1
	rm -f "$Dir/$1" 2>/dev/null
	echo 'success'
}

TryLock()
{
	echo -n "$(date) TryLock '$1' "
	Lock "$1" && echo 'success' || echo 'fail'
}

WaitLock()
{
	echo "$(date) WaitLock '$1'"
	until Lock "$1" nolog; do
		sleep 1
	done
	echo "$(date) WaitLock '$1' success"
}
