#!/bin/bash

/usr/bin/mythfilldatabase --only-update-channels
/usr/bin/mythfilldatabase --import-icon-map /home/mythtv/master_iconmap.xml --update-icon-map
echo "LOCK TABLE schemalock WRITE;" | mysql mythconverg  # Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
#myth always puts them in /.root/mythtv.  We want /home/mythtv
echo "update channel set icon=replace(icon,'/root/.mythtv','/home/mythtv')" | mysql mythconverg  # These should be shared
mkdir -p /home/mythtv/channels
mv /root/.mythtv/channels/* /home/mythtv/channels
invoke-rc.d mythtv-backend restart
