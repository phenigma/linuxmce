#!/bin/bash

if [[ "$1" == "set" ]] ;then
	shift

	USERNAME=$1
	PASSWORD=$2
	DNS_SERVICE_STRING=$3
	DOMAINS_STRING=$4

	cp /usr/pluto/teamplates/ddclient.conf.template /etc/ddclient.conf.$$
	ReplaceVars /etc/ddclient.conf
	mv /etc/ddclient.conf.$$ /etc/ddclient.conf

else

fi
