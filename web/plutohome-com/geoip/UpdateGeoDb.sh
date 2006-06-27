#!/bin/bash
cd /var/www/pluto-admin/geoip/ && \
wget -c -O GeoLiteCity.tmp.gz http://www.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz && \
gunzip GeoLiteCity.tmp.gz && \
{
	mv -f GeoLiteCity.dat{,.bak}
	mv GeoLiteCity.{tmp,dat}
}
