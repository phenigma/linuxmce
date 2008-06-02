#!/bin/sh

# Bridged mode
if [[ "`uci get network.eth0.vlan1`" == '' ]] ;then
	# With Static IP
	if [[ "`uci get network.lan.proto`" == 'static' ]] ;then
		Report_IP=`uci get network.lan.ipaddr`

	# With Dynamic IP
	else
		Report_IP=`ifconfig br-lan | grep 'inet addr:' | sed 's/.*inet addr:\([0-9.]*\).*/\1/g'`
	fi

# Routing mode
else
 	# With Static IP
	if [[ "`uci get network.wan.proto`" == 'static' ]] ;then
		Report_IP=`uci get network.wan.ipaddr`
	
	# With Dynamic IP
	else
		Report_IP=`ifconfig eth0.1 | grep 'inet addr:' | sed 's/.*inet addr:\([0-9.]*\).*/\1/g'`
	fi
fi

Report_MAC=`ifconfig eth0 | grep 'HWaddr' | sed 's/.*HWaddr \([0-9A-F:]*\).*/\1/g'`


echo "Reporting $Report_IP - $Report_MAC"
wget "http://plutohome.com/ReportAP.php?IP=${Report_IP}&MAC=${Report_MAC}"
