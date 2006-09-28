#!/bin/bash

ConfEval()
{
	local Ret=0
	local Flavor="${1:-pluto}"

	# Note to self: this "read from file into while" is absolutely necessary
	#               "cmd | while ..." spawns a subshell and our veriables will be set there instead of our current shell
	#               "while ...; do ...; done <file" works the way we need
	while read line; do
		if [[ "$line" == "#"* || "$line" == "//"* || -z "$line" ]]; then
			continue # comment line; skip
		fi
		var="${line%%=*}"
		value="${line#*=}"
		eval "export $var=\"$value\"" &>/dev/null
	#done <"/etc/MakeRelease/$Flavor.conf"
	done <"$Flavor.conf"
	return $Ret
}
