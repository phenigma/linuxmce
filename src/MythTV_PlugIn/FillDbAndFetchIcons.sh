#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

/usr/bin/mythfilldatabase --only-update-channels
/usr/bin/mythfilldatabase --import-icon-map /home/mythtv/master_iconmap.xml --update-icon-map
echo "LOCK TABLE schemalock WRITE;" | mysql mythconverg  # Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
#myth always puts them in /.root/mythtv.  We want /home/mythtv
echo "update channel set icon=replace(icon,'/root/.mythtv','/home/mythtv')" | mysql mythconverg  # These should be shared
mkdir -p /home/mythtv/channels
mv /root/.mythtv/channels/* /home/mythtv/channels

# Send a sync command to fix up the directories
/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 824

# Notify all the orbiters that myth is ready to be used
/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 12 1 406 9 "MythTV is ready.  However it may take several more minutes before the guide data is fully populated." 102 300
/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 910 9 "Finished retrieving channels"

# Now that we have channels and icons, go ahead and fill the database.  If the user interrupts this now it's ok
# because it will grab partial data and continue the next time
/usr/bin/mythfilldatabase
