#!/bin/bash

 . /usr/pluto/bin/Network_Parameters.sh

function configure_ushare() {
	local server_name="$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=187 AND FK_Device=1")"
	local server_iface="$IntIf"

	echo > /etc/ushare.conf
	echo "USHARE_NAME=$server_name"     >> /etc/ushare.conf
	echo "USHARE_IFACE=$server_iface"   >> /etc/ushare.conf
	echo "USHARE_DIR=/home/public/data" >> /etc/ushare.conf
}

function reload_ushare() {
	local pid=$(pidof ushare)
	if [[ "$pid" != "" ]] ;then
		kill $pid
	fi
	ushare --daemon
}

configure_ushare
reload_ushare
