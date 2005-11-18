#!/bin/bash

. /usr/pluto/bin/LockUtils.sh

WaitLock "PlutoDHCP" "PlutoDHCP"
/usr/pluto/bin/PlutoDHCP "$@"
Ret=$?
Unlock "PlutoDHCP" "PlutoDHCP"
exit $Ret
