#!/bin/bash

if ! grep -q 'dvb[-_]bt8xx' /etc/modules; then
	echo "dvb-bt8xx" >>/etc/modules
	modprobe dvb-bt8xx || :
fi
