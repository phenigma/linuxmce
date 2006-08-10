#!/bin/bash

## UPGRADE 2.0.0.40
## - Because /home/public/data/music changed to /home/public/data/audio
##   and /home/public/data/movies to /home/public/data/videos we neet
##   to update the user playlists

if [[ -f /etc/diskless.conf ]] ;then
	exit 0
fi

Q1="UPDATE PlaylistEntry SET Path=replace(Path,'/home/public/data/music','/home/public/data/audio') WHERE Path like '/home/public/data/music%'"
Q2="UPDATE PlaylistEntry SET Path=replace(Path,'/home/public/data/movies','/home/public/data/videos') WHERE Path like '/home/public/data/movies%'"
Q3="UPDATE File SET Path=replace(Path,'/home/public/data/music','/home/public/data/audio') WHERE Path like '/home/public/data/music%'"
Q4="UPDATE File SET Path=replace(Path,'/home/public/data/movies','/home/public/data/videos') WHERE Path like '/home/public/data/movies%'"

echo "$Q1" | mysql pluto_media
echo "$Q2" | mysql pluto_media
echo "$Q3" | mysql pluto_media
echo "$Q4" | mysql pluto_media

