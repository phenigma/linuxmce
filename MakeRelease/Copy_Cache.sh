#!/bin/bash

if [ "$#" -lt 2 ]; then
	echo "Syntax: $(basename $0) <Search path> <File 1> [<File 2> ...]"
	exit
fi

Path="$1"
FileList="$2"
Cache="/home/tmp/pluto-build/package-cache"

mkdir -p "$Cache"

x=""
y=""
for i in $(cat "$FileList"); do
	x="$x $y -name '${i}_*.deb'"
	y="-or"
done
Cmd="find \"$Path\" '(' $x ')' -exec cp '{}' \"$Cache\" ';' -print"
#eval "$Cmd"
find "$Path" '(' $x ')' -exec cp '{}' \"$Cache\" ';' -print
