#!/bin/sh

log() {
	logger -s -t 'fiire-report' $*
}

LanIP=`uci get network.lan.ipaddr`
if [[ "`uci get network.wan.proto`" == 'dhcp' ]] ;then
	WanIface=`uci get network.wan.ifname`
	WanIP=`ifconfig $WanIface | grep 'inet addr:' | sed 's/.*inet addr:\([0-9.]*\).*/\1/g'`
else
	WanIP=`uci get network.wan.ipaddr`
fi
MAC=`ifconfig eth0 | grep 'HWaddr' | sed 's/.*HWaddr \([0-9A-F:]*\).*/\1/g'`

log "Reporting lan:$LanIP wan:$WanIP mac:$MAC"
wget -qO/dev/null "http://local.fiire.com/ReportAP.php?LanIP=${LanIP}&WanIP=${WanIP}&MAC=${MAC}"
