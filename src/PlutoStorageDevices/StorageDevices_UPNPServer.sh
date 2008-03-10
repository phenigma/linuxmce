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

function configure_fuppes() {
	local server_name="$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=187 AND FK_Device=1")"
	local server_iface="$IntIf"

	sed -i "s,<!--LMCE--><dir>.*</dir>,<!--LMCE--><dir>/home/public/data</dir>,g" /etc/fuppes/fuppes.cfg
	sed -i "s,<interface>.*</interface>,<interface>$server_iface</interface>,g" /etc/fuppes/fuppes.cfg
	sed -i "s, <description_values><friendly_name>.*</friendly_name></description_values>,<description_values><friendly_name>$server_name</friendly_name></description_values>,g" /etc/fuppes/fuppes.cfg
}

function reload_fuppes() {
	/etc/init.d/fuppes restart
}

#configure_ushare
#reload_ushare

configure_fuppes
reload_fuppes
