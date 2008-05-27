#!/bin/sh

export LD_LIBRARY_PATH=/pluto/lib
rm -f /var/run/pluto.stop

exec >/dev/null 2>/dev/null </dev/null

/usr/bin/Start_DCERouter.sh &
/usr/bin/Start_ZWave.sh &
