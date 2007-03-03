#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0"

/usr/pluto/bin/Diskless_BuildDefaultImage.sh || echo "WARNING: Diskless_BuildDefaultImage.sh returned an error!"

# summary of how this script can be called:
#        * <postinst> `configure' <most-recently-configured-version>
if [[ "$2" != "" ]] ;then
	/usr/pluto/bin/Diskless_CreateTBZ.sh || echo "WARNING: Diskless_CreateTBZ.sh returned an error!"
fi
