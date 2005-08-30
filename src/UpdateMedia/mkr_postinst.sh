#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

# One time also update the thumbnails and search tokens
/usr/pluto/bin/UpdateMedia -d -t -s /home/public/data/documents
/usr/pluto/bin/UpdateMedia -d /home/public/data/movies
/usr/pluto/bin/UpdateMedia -d /home/public/data/music
/usr/pluto/bin/UpdateMedia -d /home/public/data/pictures
/usr/pluto/bin/UpdateMedia -d /home/public/data/videos

Q="SELECT PK_Users FROM Users"
R="$(RunSQL "$Q")"

for User in $R; do
	Dir="/home/user_$User/data/documents"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir"
	fi
	Dir="/home/user_$User/data/movies"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir"
	fi
	Dir="/home/user_$User/data/music"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir"
	fi
	Dir="/home/user_$User/data/pictures"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir"
	fi
	Dir="/home/user_$User/data/videos"
	if [[ -d "$Dir" ]]; then
		/usr/pluto/bin/UpdateMedia -d "$Dir"
	fi
done
