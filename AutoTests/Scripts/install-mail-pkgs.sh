#!/bin/bash

exec &> >(tee /var/installmail)

version=$(cat /etc/debian_version)
if [[ "$version" == "4.0" ]] ;then
	ExtraSources="deb http://ro.archive.ubuntu.com/ubuntu/ feisty main restricted universe multiverse
	deb http://ro.archive.ubuntu.com/ubuntu/ feisty-security main restricted universe multiverse
	deb http://ro.archive.ubuntu.com/ubuntu/ feisty-updates  main restricted universe multiverse"

	if ! grep -q 'http://ro.archive.ubuntu.com/ubuntu/' /etc/apt/sources.list ;then
		echo "$ExtraSources" >> /etc/apt/sources.list
		apt-get update
	fi
fi
#/tmp/AutoTests/Scripts/debconf-exim4.sh

apt-get install --force-yes -y guile-1.6-libs libguile-ltdl-1 libmailutils1 libqthreads-12
apt-get install --force-yes -y exim4
apt-get install --force-yes -y mailutils
