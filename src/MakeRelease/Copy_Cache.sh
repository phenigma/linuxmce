#!/bin/bash

if [ "$#" -lt 2 ]; then
	echo "Syntax: $(basename $0) <Search path> <File 1> [<File 2> ...]"
	exit
fi

Path="$1"; shift
Cache="/home/tmp/pluto-build/package-cache"

mkdir -p "$Cache"

x=""
y=""
for i in $(cat "$@"); do
	x="$x $y -name '${i}_*.deb'"
	y="-or"
done
Cmd="find \"$Path\" '(' $x ')' -exec cp '{}' \"$Cache\" ';' -print"
eval "$Cmd"
