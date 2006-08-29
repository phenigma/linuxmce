#!/bin/bash

COMP_MODULES_DIR="$(pwd)/compiledModules"
KSRC_STORAGE_DIR="/home/src"

mkdir -p "$compiledModulesDir"

function BuildModules() {
	echo "# Building for kernel $KVER"
	export KVER="$1"
	export KDIR="$KSRC_STORAGE_DIR/linux-$KVER"
	export KSRC="$KDIR"
	export KERNEL="$KVER"

	if [[ ! -d "$KDIR" ]] ;then
		echo "# Cannot find kernel sources of $KVER in $KDIR"
		exit 1
	fi

	## Build specific
	make clean
	make -C driver
	## Build specific

	mkdir -p "$COMP_MODULES_DIR/$KVER/extra"
	find '(' -type d -name 'compiledModules' -prune -false ')' -or -name '*.ko' -exec cp  '{}' "$COMP_MODULES_DIR/$KVER/extra" ';'
}

K_RUNNING=$(uname -r)

BuildModules "$K_RUNNING"
BuildModules "$K_RUNNING-smp"
