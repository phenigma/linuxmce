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

