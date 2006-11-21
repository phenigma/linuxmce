#!/bin/sh

if [ ! -d src/libffmpeg ]
then
	echo "please run this script from the root of the source tree"
	echo "of the unstripped upstream sources!"
	exit
fi

mv src/libffmpeg/libavcodec/libpostproc src &&
rm -rf src/libffmpeg &&
rm -rf src/libfaad &&
rm -rf src/libmad &&

patch -Np1 -i debian/ubuntu2.diff &&

AUTOMAKE=automake-1.9 ACLOCAL=aclocal-1.9 autoreconf -fiv &&
./configure &&
make dist &&
echo "successfully stripped faad and ffmpeg"

