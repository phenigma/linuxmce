#!/bin/bash

set -ex

if [[ -z "$1" ]]; then
	echo "You need to specify a build directory"
	exit 1
fi

if [[ -z "$MakeRelease_Flavor" ]]; then
	echo "'MakeRelease_Flavor' variable not found"
	exit 1
fi

BUILD_DIR="$1"

pushd "$BUILD_DIR" >/dev/null
mkdir -p debian/main/binary-i386
mv tmp/*.deb debian/main/binary-i386/
rm -rf tmp/
popd >/dev/null

pushd "$BUILD_DIR/debian/" >/dev/null
dpkg-scanpackages . /dev/null | sed 's,\./,dists/'"$maindeb"'/,g' | gzip -9c > main/binary-i386/Packages.gz
popd >/dev/null

mv "$BUILD_DIR/debian-packages.tmp" "$BUILD_DIR/debian/debian-packages.list"
