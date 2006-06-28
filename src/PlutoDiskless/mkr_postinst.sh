#!/bin/bash

## This is safe to be runned at every install/upgrade
/usr/pluto/bin/Diskless_Setup.sh --skiplock || /bin/true
/usr/pluto/bin/DHCP_config.sh || /bin/true
/usr/pluto/bin/Diskless_ExportsNFS.sh || /bin/true
