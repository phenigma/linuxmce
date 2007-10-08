#!/bin/bash

. /usr/pluto/bin/X-Modelines.sh

BuiltInResolutions=(640x480 800x600 1024x768 1280x1024 1600x1200)
BuiltInRefreshRates=(60 75)

GenModeline()
{
	local ResX="$1" ResY="$2" Refresh="${3:-60}" ScanType="$4"
	local Modeline
	local VarName

	VarName="Modeline_${ResX}x${ResY}"
	if [[ "$ScanType" == interlace ]]; then
		VarName="${VarName}i"
	fi
	VarName="${VarName}_${Refresh}"

	if [[ -n "${!VarName}" ]]; then
		# use a hardcoded^W predefined modline if exists (also overrides built-in modes)
		Modeline="${!VarName}"
	elif [[ "$ScanType" != interlace && " ${BuiltInResolutions[*]} " == *" ${ResX}x${ResY} "* && " ${BuiltInRefreshRates[*]} " == *" ${Refresh} "* ]]; then
		# built in
		Modeline=
	else
		# generate a modeline otherwise
		Modeline=$(/usr/pluto/bin/xtiming.pl "$ResX" "$ResY" "$Refresh" "$ScanType")
		Modeline="${Modeline/@*\"/\"}"
	fi

	echo "$Modeline"
}

TestXConfig()
{
	local Display="${1:-0}" ConfigFile="${2:-/etc/X11/xorg.conf}"
	X :$((Display+2)) -ignoreABI -probeonly -config "$ConfigFile" -logverbose 9 &>/dev/null
	return $?
}
