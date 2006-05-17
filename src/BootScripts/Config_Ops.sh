#!/bin/bash

NoSpace="s/ //g"
LogSectionDelimiter="========== NEW LOG SECTION =========="

. /usr/pluto/bin/LockUtils.sh

ConfEval()
{
	local Ret=0
	WaitLock "pluto.conf" "Config_Ops-ConfEval" nolog
	if [[ ! -e /etc/pluto.conf ]]; then
		Unlock "pluto.conf" "Config_Ops-ConfEval" nolog
		exit 0
	fi
	# Note to self: this "read from file into while" is absolutely necessary
	#               "cmd | while ..." spawns a subshell and our veriables will be set there instead of our current shell
	#               "while ...; do ...; done <file" works the way we need
	while read line; do
		line="${line// }"
		if [[ "$line" == "#"* || "$line" == "//"* ]]; then
			continue # comment line; skip
		fi
		eval "export $line" &>/dev/null
	done </etc/pluto.conf
	Unlock "pluto.conf" "Config_Ops-ConfEval" nolog
	return $Ret
}

ConfSet()
{
	local Ret=0
	WaitLock "pluto.conf" "Config_Ops-ConfSet" nolog
	local Variable="$1" Value="$2"
	local Line="$Variable = $Value"
	if ! grep "$Variable.*=" /etc/pluto.conf &>/dev/null; then
		echo "$Line" >> /etc/pluto.conf
	else
		sed -i "s/^.*$Variable.*=.*$/$Line/g" /etc/pluto.conf
	fi
	eval "export $Variable=\"$Value\"" &>/dev/null
	Unlock "pluto.conf" "Config_Ops-ConfSet" nolog
	return $Ret
}

ConfGet()
{
	local Ret=0
	WaitLock "pluto.conf" "Config_Ops-ConfGet" nolog
	local Variable="$1" Line
	if ! grep "$Variable.*=" /etc/pluto.conf &>/dev/null; then
		Ret=1
	else
		Line=$(grep "$Variable.*=" /etc/pluto.conf 2>/dev/null | head -1 | sed "$NoSpace")
		eval "export $Line"
	fi
	Unlock "pluto.conf" "Config_Ops-ConfGet" nolog
	return $Ret
}

ReplaceVars()
{
	# TODO
	local File Commands VarValue SedCmd
	File="$1"
	
	for i in $Vars; do
		eval "VarValue=\"\$$i\""
		VarValue=${VarValue//\//\\\/}
		VarValue=$(echo "$VarValue" | sed 's/^ *//g; s/ *$//g')
		SedCmd="s/%$i%/$VarValue/g"
		[[ -z "$Commands" ]] && Commands="$SedCmd" || Commands="$Commands; s/%$i%/$VarValue/g"
	done
	sed -i "$Commands" $File
}

PackageIsInstalled()
{
	local Pkg="$1"

	[[ -z "$Pkg" ]] && return 1
	#dpkg -s "$Pkg" 2>/dev/null | grep -q 'Status: install ok installed'
	PackageStatus "$Pkg" | grep -q 'Status: install ok installed'
}

PackageStatus()
{
	local Pkg="$1"

	[[ -z "$Pkg" ]] && return 1
	awk "/^Package: $Pkg/,/^$/" /var/lib/dpkg/status
}

InstalledPackages()
{
	local Pkg InstPkg=""

	for Pkg in "$@"; do
		if PackageIsInstalled "$Pkg"; then
			InstPkg="$InstPkg $(PackageStatus $Pkg | grep '^Package: ' | sed 's/^Package: //')"
		fi
	done
	echo "${InstPkg# }"
}

ConfEval

VGcmd=""
if [[ -n "$Valgrind" ]]; then
	if which valgrind &>/dev/null; then
		VGcmd="valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=15 " # trailing space is mandatory
		export GLIBCXX_FORCE_NEW=1
		export GLIBCPP_FORCE_NEW=1
	else
		echo "*** WARNING *** Valgrind not installed. Running programs the normal way ***"
	fi
fi

[[ -f /tmp/pluto.gdb ]] || echo "run" >/tmp/pluto.gdb
if [[ -n "$GDB" ]]; then
	if which gdb &>/dev/null; then
		VGcmd="gdb --command=/tmp/pluto.gdb --args "
	elif [[ -z "$VGcmd" ]]; then
		echo "*** WARNING *** GDB not installed. Running programs the normal way ***"
	fi
fi

ulimit -c unlimited
ulimit -n 4096
shopt -s nullglob
export HOME=/root
