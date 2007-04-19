#!/bin/bash

exec &>/dev/null </dev/null
export DISPLAY=:0
echo ""
echo "Linux MCE Installer Starting ... please wait"
echo ""

# create a temp directory to extract to.
export WRKDIR=`mktemp -d /tmp/selfextract.XXXXXX`

SKIP=`awk '/^__ARCHIVE_FOLLOWS__/ { print NR + 1; exit 0; }' $0`

# Take the TGZ portion of this file and pipe it to tar.
tail -n +$SKIP $0 | tar xz -C $WRKDIR

# execute the installation script

PREV=`pwd`
cd $WRKDIR
export LD_LIBRARY_PATH=$WRKDIR
echo "$WRKDIR" > /etc/ld.so.conf.d/mce-installer.conf
ldconfig
./mce-installer


# delete the temp files
cd $PREV
rm -rf $WRKDIR
rm -rf /etc/ld.so.conf.d/mce-installer.conf

exit 0

__ARCHIVE_FOLLOWS__
