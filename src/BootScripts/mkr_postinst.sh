#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

## Update the PlutoVersion variable from pluto.conf
Version=$(dpkg -s pluto-boot-scripts | grep '^Version:' | sed  's/Version: //')
ConfSet "PlutoVersion" "$Version"

## Generate the /etc/apt/apt.conf.d/30pluto file
rm -rf /var/cache/polipo/*
pluto_apt_conf='// Pluto apt conf add-on
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
Acquire::http::timeout "10";
Acquire::ftp::timeout "10";
'
echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto

## Ouch 
chmod 777 /usr/pluto/locks

## Copy our logrotate policy (every package should have his own policy but we are from pluto :)
cp /usr/pluto/templates/logrotate.pluto.tmpl /etc/logrotate.d/pluto

