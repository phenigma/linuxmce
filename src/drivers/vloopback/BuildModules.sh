#!/bin/bash

COMP_MODULES_DIR="$(pwd)/compiledModules"

mkdir -p "$compiledModulesDir"

function BuildModules() {
	echo "# Building for kernel $KVER"
	export KVER="$1"
	export KDIR="/lib/modules/$KVER/build"
	export KSRC="$KDIR"
	export KERNEL="$KVER"

	if [[ ! -d "$KDIR" ]] ;then
		echo "# Cannot find kernel sources of $KVER in $KDIR"
		exit 1
	fi

	## Build specific
	make clean
	make
	## Build specific

	mkdir -p "$COMP_MODULES_DIR/$KVER/misc"
	find '(' -type d -name 'compiledModules' -prune -false ')' -or -name '*.ko' -exec cp  '{}' "$COMP_MODULES_DIR/$KVER/misc" ';'
}

if [[ "$MakeRelease_Kernel" == "" ]] ;then
	echo "ERROR: No kernel to build for!"
	exit 1
fi

echo "#!/bin/bash" > mkr_postinst.sh
for KERN in ${MakeRelease_Kernel} ;do
	BuildModules "$KERN"
	echo "depmod \"$KERN\" 2>/dev/null || /bin/true" >> mkr_postints.sh
done
chmod +x mkr_postinst.sh
