#!/bin/bash

function Replace_Mirror {
	local mirror=$1
	local flag='0'
	local newlines="
deb file:/usr/pluto/deb-cache/ ./
deb $mirror feisty main restricted universe multiverse
deb $mirror feisty-security main restricted universe multiverse
deb $mirror feisty-updates  main restricted universe multiverse
deb http://linuxmce.com/ununtu ./
"

	while read line ;do
		if [[ "$line" == '# Choosed mirror - end' ]] ;then
			echo "$newlines" >> /etc/apt/sources.list.$$
			flag='0'
		fi
		if [[ "$line" == '# Choosed mirror - start' ]] ;then
			echo "$line" >> /etc/apt/sources.list.$$		
			flag='1'
		fi
		if [[ "$flag" == '0' ]] ;then
			echo "$line" >> /etc/apt/sources.list.$$
		fi
	done < /etc/apt/sources.list

	mv -f /etc/apt/sources.list.$$ /etc/apt/sources.list
}


function Setup_Apt_Conffiles {	
	## Backup initial apt sources.list
	if [ ! -e /etc/apt/sources.list.pbackup ] ;then
		cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
	fi

	## Setup apt sources.list
	local Sources="# Pluto sources - start
# Choosed mirror - start
deb file:/usr/pluto/deb-cache/ ./
deb $c_installMirror feisty main restricted multiverse universe
deb $c_installMirror feisty-security main restricted multiverse universe
deb $c_installMirror feisty-updates main restricted multiverse universe
deb http://linuxmce.com/ubuntu ./
# Choosed mirror - end
#deb http://10.0.0.82/ ./
# Pluto sources - end"

	if ! grep -qF '# Pluto sources - start' /etc/apt/sources.list ;then
		echo "$Sources" > /etc/apt/sources.list.$$
		cat /etc/apt/sources.list.$$ /etc/apt/sources.list > /etc/apt/sources.list.$$.all
		mv -f /etc/apt/sources.list.$$.all /etc/apt/sources.list
		rm -f /etc/apt/sources.list.$$
	else
		Replace_Mirror "$c_installMirror"
	fi

	local SourcesOffline="# Pluto sources offline - start
deb file:/usr/pluto/deb-cache/ ./
# Pluto sources offline - end"
	echo "$SourcesOffline" >/etc/apt/sources.list.offline

	# Setup pluto's apt.conf
	pluto_apt_conf='// Pluto apt conf add-on
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
Acquire::http::timeout "10";
Acquire::ftp::timeout "10";
APT::Get::AllowUnauthenticated "true";
'
	echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto

	ln -s /usr/pluto/var/apt.conf.offline /etc/apt/apt.conf.d/99offline

	./mce-installer-preseed.sh
	apt-get update
}

function Setup_NIS {
	## Put a temporary nis config file that will prevent ypbind to start
	echo "
NISSERVER=false
NISCLIENT=false
YPPWDDIR=/etc
YPCHANGEOK=chsh
NISMASTER=
YPSERVARGS=
YPBINDARGS=
YPPASSWDDARGS=
YPXFRDARGS=
" > /etc/default/nis
}
