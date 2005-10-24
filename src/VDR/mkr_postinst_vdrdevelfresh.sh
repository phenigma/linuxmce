#!/bin/bash
#
# This script helps to automate a repository change.
# It just reinstalls all plugins that have been installed 
# after changing sources.list to point to a new repository.
# After calling apt-get update, you can call this script
# to refresh vdrdevel and any plugins
#

# these prevent the package from installing as a post inst
#if [ "$#" != "0" ] && [ "$1" != "--dry-run" ]; then
#    echo "just knows --dry-run or nothing"
#    exit 1
#fi
 
#(apt-get clean; apt-get $1 install --reinstall vdrdevel ; apt-get $1 install --reinstall `(export COLUMNS=512; dpkg -l | tee /tmp/test.lst | grep "ii  vdrdevel-plugin" | cut -d" " -f3)`)
#/etc/init.d/vdrdevel stop

cd /var/lib/vdrdevel
cp SETTINGS/etcdefault/vdrdevel /etc/default/.
cp SETTINGS/etcvdrdevel/svdrphosts.conf /etc/vdrdevel/.
cp SETTINGS/etcvdrdevel/plugin.xine.conf /etc/vdrdevel/plugins/.
cp SETTINGS/initd/dvb /etc/init.d/.
cp SETTINGS/initd/vdrdevel /etc/init.d/.
cp SETTINGS/varlibvdrdevel/remote.KBD.conf /var/lib/vdrdevel/.
cp SETTINGS/varlibvdrdevel/setup.conf /var/lib/vdrdevel/.

/etc/init.d/dvb start
/etc/init.d/vdrdevel start

