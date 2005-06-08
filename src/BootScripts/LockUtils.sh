#!/bin/bash

# TODO: real locks, with PID and locking host inside to detect stale locks

Dir="/usr/pluto/locks"

Lock()
{
	local Lock="$1"
	local Device="$2"
	local NoLog="$3"
	
	[[ -z "$NoLog" ]] && echo "$$ $(date) Lock '$Lock' ($Device)"

	[ -z "$Lock" ] && return 1
	if ln -s "/proc/$$-$Device" "$Dir/$Lock" 2>/dev/null; then
		[[ -z "$NoLog" ]] && echo "$$ $(date) Lock '$Lock' ($Device) fail"
		return 0
	else
		[[ -z "$NoLog" ]] && echo "$$ $(date) Lock '$Lock' ($Device) success"
		return 1
	fi
}

# there is no ability to check if this our lock yet
Unlock()
{
	local Lock="$1"
	local Device="$2"
	
	echo "$$ $(date) Unlock '$Lock' ($Device)"

	[ -z "$Lock" ] && echo "$$ $(date) Unlock '$Lock' ($Device) fail" && return 1
	rm -f "$Dir/$Lock" 2>/dev/null
	echo "$$ $(date) Unlock '$Lock' ($Device) success"
	return 0
}

TryLock()
{
	local Lock="$1"
	local Device="$2"

	echo "$$ $(date) TryLock '$Lock' ($Device)"
	if Lock "$Lock" "$Device"; then
		echo "$$ $(date) TryLock '$Lock' ($Device) success"
		return 0
	else
		echo "$$ $(date) TryLock '$Lock' ($Device) fail"
		return 1
	fi
}

WaitLock()
{
	local Lock="$1"
	local Device="$2"
	
	echo "$$ $(date) WaitLock '$Lock' ($Device)"
	until Lock "$1" "$Device" nolog; do
		sleep 1
	done
	echo "$$ $(date) WaitLock '$Lock' ($Device) success"
	return 0
}
