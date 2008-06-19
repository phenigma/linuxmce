#!/bin/sh

log() {
	logger -s -t 'fiire-report' $*
}

LanIP=`uci get network.lan.ipaddr`
WanIP=`uci get network.wan.ipaddr`
MAC=`ifconfig eth0 | grep 'HWaddr' | sed 's/.*HWaddr \([0-9A-F:]*\).*/\1/g'`

log "Reporting lan:$LanIP wan:$WanIP mac:$MAC"
wget -qO/dev/null "http://local.fiire.com/ReportAP.php?LanIP=${LanIP}&WanIP=${WanIP}&MAC=${MAC}"
