#!/bin/bash

REPO_HOME=/home/samba/repositories/pluto/replacements/main/binary-i386

# make -C ivtv/ clean 
# make -C ivtv/ deb

# make -f Makefile.pluto -C video4linux clean 
# make -f Makefile.pluto -C video4linux deb


if [ -d "$REPO_HOME" ]; then
	cp ivtv/*.deb "$REPO_HOME";
	cp video4linux/*.deb "$REPO_HOME";
	pushd "$REPO_HOME";
	./update-repository
	popd
else
	echo "No repository available in \"$REPO_HOME\" not updating";
fi;
