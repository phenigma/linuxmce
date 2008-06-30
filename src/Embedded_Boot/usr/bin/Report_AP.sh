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
wget -qO/dev/null "http://cisco.fiire.com/ReportAP.php?IP=${WanIP}&MAC=${MAC}"

# Update /etc/hosts
grep -v local.fiire.com /etc/hosts > /etc/hosts.temp
echo "$LanIP local.fiire.com" >> /etc/hosts.temp
mv /etc/hosts.temp /etc/hosts

echo "$(date -R) Reported AP $LanIP, $WanIP, $MAC" >>/tmp/log.Report_AP
/etc/init.d/dnsmasq restart
