#!/bin/bash

ConfFile="/var/www/PlutoAdminConfig/config.inc.php"

Conf="<?
  \$dbUser = 'root';
  \$dbPass = '';
  \$dbDatabase = 'pluto_main';
  \$dbServer = 'localhost';
  \$dbType = 'mysql';

  \$dbPlutoMediaType = 'MySQL';
  \$dbPlutoMediaUser = 'plutomedia';
  \$dbPlutoMediaPass = '';
  \$dbPlutoMediaServer = 'localhost';
  \$dbPlutoMediaDatabase = 'pluto_media';
?>"

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

Redirect="<?php header('Location: pluto-support/index.php?section=pluto'); ?>"
[ -e /var/www/index.php ] || echo "$Redirect" >/var/www/index.php
