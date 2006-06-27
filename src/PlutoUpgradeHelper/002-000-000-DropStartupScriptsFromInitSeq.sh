#!/bin/bash

## Remove Startup_Scripts.sh from rc scripts as is not used any more
update-rc.d -f Startup_Scripts.sh remove || /bin/true
