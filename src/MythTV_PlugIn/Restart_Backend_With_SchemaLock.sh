#!/bin/bash

echo "LOCK TABLE schemalock WRITE;" | mysql mythconverg  # Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
/etc/init.d/mythtv-backend restart