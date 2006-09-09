#!/bin/bash

COMP_MODULES_DIR="$(pwd)/compiledModules"
KSRC_STORAGE_DIR="/home/src"

mkdir -p "$compiledModulesDir"

function BuildModules() {
	echo "# Building for kernel $KVER"
	export KVER="$1"
	export KDIR="$KSRC_STORAGE_DIR/linux-$KVER"
	export KSRC="$KDIR"

	if [[ ! -d "$KDIR" ]] ;then
		echo "# Cannot find kernel sources of $KVER in $KDIR"
		exit 1
	fi

	## Build specific
	./configure --with-kernel=$KDIR --with-cards=all --with-card-options=all --with-oss=yes --enable-dynamic-minors
	make clean
	make
	## Build specific

	mkdir -p "$COMP_MODULES_DIR/$KVER/misc"
	find '(' -type d -name 'compiledModules' -prune -false ')' -or -name '*.ko' -exec cp  '{}' "$COMP_MODULES_DIR/$KVER/misc" ';'
}

K_RUNNING=$(uname -r)

BuildModules "$K_RUNNING"
BuildModules "$K_RUNNING-smp"

echo "#!/bin/bash
depmod "$K_RUNNING"
depmod "$K_RUNNING-smp"
" >mkr_postinst.sh
chmod +x mkr_postinst.sh
