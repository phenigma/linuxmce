#!/bin/bash

## This is safe to be runned at every install/upgrade
/usr/pluto/bin/Diskless_Setup.sh --skiplock
/usr/pluto/bin/DHCP_config.sh
/usr/pluto/bin/Diskless_ExportsNFS.sh
