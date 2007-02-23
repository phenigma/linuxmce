#!/bin/bash

/usr/pluto/bin/Diskless_BuildDefaultImage.sh || echo "WARNING: Diskless_BuildDefaultImage.sh returned an error!"
/usr/pluto/bin/Diskless_CreateTBZ.sh || echo "WARNING: Diskless_CreateTBZ.sh returned an error!"
