#!/bin/bash

[ "x$1" = "x" ] && echo You need to specify a build directory && exit;

BUILD_DIR=$1

pushd "$BUILD_DIR"
mkdir -p debian/main/binary-i386
mv tmp/*.deb debian/main/binary-i386/
rm -rf tmp/
popd

pushd "$BUILD_DIR/debian/"

dpkg-scanpackages . /dev/null | sed 's,\./,dists/20dev/,g' | gzip -9c > main/binary-i386/Packages.gz
popd

mv "$BUILD_DIR/debian-packages.tmp" "$BUILD_DIR/debian/debian-packages.list"
