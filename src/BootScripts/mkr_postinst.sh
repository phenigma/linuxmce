#!/bin/bash

update-rc.d -f Startup_Scripts.sh remove || /bin/true
update-rc.d Startup_Scripts.sh defaults 22 10

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# /etc/apt/apt.conf.d/30pluto
rm -rf /var/cache/polipo/*
pluto_apt_conf='// Pluto apt conf add-on
//Acquire::http::Proxy "http://'"$DCERouter"':8123";
//Acquire::ftp::Proxy "ftp://'"$DCERouter"':8124";
//Acquire::ftp::ProxyLogin { "USER $(SITE_USER)@$(SITE):$(SITE_PORT)"; "PASS $(SITE_PASS)"; };
//Acquire::http::Proxy::dcerouter "DIRECT";
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
'

echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto
