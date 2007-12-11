#!/bin/bash

if [[ "$1" != "demonized" ]] ;then
	screen -S ReportErrors -d -m /bin/bash $0 demonized
	exit 0
fi

set -x
kill $(cat /var/loop.pid)

# let's give time to others to get this script and try to execute it
sleep 60

# ready to gather info
/bin/bash /tmp/AutoTests/Scripts/DisableOrbiterSimulator.sh
/bin/bash /tmp/AutoTests/Scripts/PlayedMedia.sh
/bin/bash /tmp/AutoTests/Scripts/MediaPlugin.sh
/bin/bash /tmp/AutoTests/Scripts/CoreDumpsStat.sh
/bin/bash /tmp/AutoTests/Scripts/SumarizeErrors.sh
/bin/bash /tmp/AutoTests/Scripts/Unhandled.sh
/bin/bash /tmp/AutoTests/Scripts/DeviceDied.sh
/bin/bash /tmp/AutoTests/Scripts/SendEmail.sh
/bin/bash /tmp/AutoTests/Scripts/PutBackLogRotate.sh
/bin/bash /tmp/AutoTests/Scripts/BackupCoredumps.sh
