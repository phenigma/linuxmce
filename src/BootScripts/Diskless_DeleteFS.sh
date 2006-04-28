#!/bin/bash

IP="$1"
MAC="$2"
lcdMAC=$(echo ${MAC//:/-} | tr 'A-Z' 'a-z')

if [[ -z "$IP" ]]; then
	echo "Error: Empty IP"
	exit
fi

echo "Deleting MD files. MAC=$MAC IP=$IP"

echo "Deleting SSH host key from Core"
sed -i "/$IP/ d" /root/.ssh/known_hosts &>/dev/null

echo "Filesystem"
rm -rf /home/diskless/"$IP"

echo "TFTP"
rm -rf /tftpboot/"$IP"
rm -rf /tftpboot/pxelinux.cfg/"01-$lcdMAC"
