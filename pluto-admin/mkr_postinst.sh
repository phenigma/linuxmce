#!/bin/bash

mkdir -p -m 0777 /usr/pluto/orbiter/floorplans
ln -sf /usr/pluto/orbiter/floorplans/ /var/www/pluto-admin/
mkdir -p /usr/pluto/orbiter/users
ln -sf /usr/pluto/orbiter/floorplans/ /var/www/floorplans
ln -sf /usr/pluto/orbiter/users/ /var/www/users

mkdir -p -m 0777 /home/mediapics
ln -sf /home/mediapics /var/www/pluto-admin/mediapics

Index=$(grep DirectoryIndex /etc/apache/httpd.conf | sed 's/DirectoryIndex//g; s/^ *//g; s/ *$//g' | grep -v '^#')
[ -n "$Index" ] || exit 0

for File in $Index; do
	[ -e "/var/www/$File" -a "$File" != "index.html" ] && exit 0
done
[ -e "/var/www/index.html" ] && ! diff /usr/share/apache/default-configs/apache/intro.html /var/www/index.html &>/dev/null && exit 0
rm -f /var/www/index.html

Redirect="<?php header('Location: support/index.php?section=home&package=0'); ?>"
[ -e /var/www/index.php ] || echo "$Redirect" >/var/www/index.php
