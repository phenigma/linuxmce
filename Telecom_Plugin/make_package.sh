#!/bin/bash

#<-dceag-mp-b->
#DCE_Package
#DCE_TargetDir

if [ "$#" != 2 ]; then
	echo "Syntax: $0 <application name> <version>"
	exit 1
fi

PACK_LOC=../Packages
ORIG_APPNAME="$1"
APPNAME=pluto-`echo "$ORIG_APPNAME" | tr 'A-Z_' 'a-z\-'`
APPVER="$2"

if [ ! -d "$PACK_LOC" ]; then
	mkdir -p "$PACK_LOC"
fi

PACK_NAME="$APPNAME-$APPVER"
PACK_DIR="$PACK_LOC/$PACK_NAME"

if [ ! -d "$PACK_DIR" ]; then
	cp -a "$PACK_LOC/_Package_template" "$PACK_DIR"
fi

BIN_LOC="../../bin"
LIB_LOC="../../lib"
BIN_FILE="$BIN_LOC/$ORIG_APPNAME"
PLUGIN_FILE="$BIN_LOC/$ORIG_APPNAME.so"
LIB_FILE="$LIB_LOC/lib$ORIG_APPNAME.a"

pushd "$PACK_DIR"

mkdir -p pluto/bin
mkdir -p pluto/lib

echo "-- $BIN_FILE"
if [ -f "$BIN_FILE" ]; then
	echo "Found bin file"
	cp "$BIN_FILE" root/pluto/bin
fi

echo "-- $PLUGIN_FILE"
if [ -f "$PLUGIN_FILE" ]; then
	echo "Found plugin file"
	cp "$PLUGIN_FILE" root/pluto/bin
fi

echo "-- $LIB_FILE"
if [ -f "$LIB_FILE" ]; then
	echo "Found lib file"
	cp "$LIB_FILE" root/pluto/lib
fi

popd

pushd "$PACK_LOC"
./propagate.pl "$PACK_NAME"
popd

#<-dceag-mp-e->

