#!/bin/bash

/usr/bin/mythfilldatabase
/usr/bin/mythfilldatabase --import-icon-map /home/mythtv/master_iconmap.xml --update-icon-map
echo "LOCK TABLE schemalock WRITE;" | mysql mythconverg  # Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
/etc/init.d/mythtv-backend restart
