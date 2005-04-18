update-rc.d Startup_Scripts.sh defaults 99 10

# /etc/apt/apt.conf.d/30pluto
pluto_apt_conf='// Pluto apt conf add-on
Acquire::http::Proxy "http://dcerouter:8123";
Acquire::ftp::Proxy "ftp://dcerouter:8124";
Acquire::ftp::ProxyLogin { "USER $(SITE_USER)@$(SITE):$(SITE_PORT)"; "PASS $(SITE_PASS)"; };
Acquire::http::Proxy::dcerouter "DIRECT";
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
'

echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto
