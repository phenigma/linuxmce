#!/bin/bash

. /usr/pluto/bin/X-CleanupVideo.sh
CleanupVideo
/usr/pluto/bin/Xconfigure.sh --update-video-driver
/usr/pluto/bin/Xconfigure.sh
