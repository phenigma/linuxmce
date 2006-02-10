#!/bin/bash


#echo syncing pluto 2 amp for embedded phone
if [ -x /usr/pluto/bin/sync_pluto2amp.pl ]
then
	/usr/pluto/bin/sync_pluto2amp.pl
fi

export SDL_VIDEO_X11_WMCLASS="Orbiter"
exec ./Orbiter "$@"

