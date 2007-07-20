#!/bin/bash

exec 3>&1 1>&2

. /usr/pluto/bin/LockUtils.sh

WaitLock "PlutoDHCP" "PlutoDHCP.sh"
/usr/pluto/bin/PlutoDHCP "$@" >&3
Ret=$?
Unlock "PlutoDHCP" "PlutoDHCP.sh"

/usr/pluto/bin/DHCP_config.sh

exec 1>&3 3>&-
exit $Ret
