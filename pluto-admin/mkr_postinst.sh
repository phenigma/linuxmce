#!/bin/bash

mkdir -p $(/usr/bin/dirname "$ConfFile")
echo "$Conf" >"$ConfFile"
mkdir -p /usr/pluto/orbiter/floorplans
mkdir -p /usr/pluto/orbiter/users
ln -sf /usr/pluto/orbiter/floorplans/ /var/www/floorplans
ln -sf /usr/pluto/orbiter/users/ /var/www/users

Index=$(grep DirectoryIndex /etc/apache/httpd.conf | sed 's/DirectoryIndex//g; s/^ *//g; s/ *$//g')
[ -n "$Index" ] || exit 0

for File in $Index; do
	[ -e "$File" ] && exit 0
done

Redirect="<?php header('Location: support/index.php?section=pluto'); ?>"
[ -e /var/www/index.php ] || echo "$Redirect" >/var/www/index.php

mkdir -p -m 0777 /usr/pluto/orbiter/floorplans
ln -sf /usr/pluto/orbiter/floorplans /var/www/pluto-admin/
