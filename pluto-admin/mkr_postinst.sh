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

