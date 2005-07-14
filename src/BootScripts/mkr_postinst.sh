update-rc.d Startup_Scripts.sh defaults 99 10

. /usr/pluto/bin/Config_Ops.sh

# /etc/apt/apt.conf.d/30pluto

pluto_apt_conf='// Pluto apt conf add-on
//Acquire::http::Proxy "http://'"$DCERouter"':8123";
//Acquire::ftp::Proxy "ftp://'"$DCERouter"':8124";
//Acquire::ftp::ProxyLogin { "USER $(SITE_USER)@$(SITE):$(SITE_PORT)"; "PASS $(SITE_PASS)"; };
//Acquire::http::Proxy::dcerouter "DIRECT";
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
'

echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto

# Changed from 2.0.0.24 to 2.0.0.25: pluto logs and core dumps were moved to /home
if [ ! -L /usr/pluto/coredumps -a -d /usr/pluto/coredumps ]; then
	mv /usr/pluto/coredumps /home
	mkdir -p /home/coredumps
	ln -sf /home/coredumps /usr/pluto/coredumps
fi

if [ ! -L /var/log/pluto -a -d /var/log/pluto ]; then
	mkdir -p /home/logs
	mv /var/log/pluto /home/logs
	mkdir -p /home/logs/pluto
	ln -sf /home/logs/pluto /var/log/pluto
fi
