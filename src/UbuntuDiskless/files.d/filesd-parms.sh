#!/bin/bash

# Parse parameters for files.d scripts

for ((i = 1; i <= "$#"; i++)); do
	Parm="${!i}"
	case "$Parm" in
		--root) ((i++)) RootLocation="${!i}" ;;
		--device) ((i++)) Device="${!i}" ;;
		*) echo "ERR: Unknown parameter '$Parm'"; exit 1 ;;
	esac
done

if [[ ! -d "$RootLocation" ]] ;then
	echo "ERROR: Directory "$RootLocation" does not exist"
	exit 1
fi
