#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Syntax: $0 <IP> <MAC>"
	exit 1
fi

IP="$1"
MAC="$2"

FSarchive=PlutoMD.tar.bz2
HexIP=$(gethostip -x "$IP")

echo "Extracting filesystem for diskless client '$IP,$MAC'"

mkdir -p /usr/pluto/diskless/"$IP"
cd /usr/pluto/diskless/"$IP"
tar -xjvf /usr/pluto/install/"$FSarchive"
echo "IP=$IP" >etc/diskless.conf
echo "MAC=$MAC" >>etc/diskless.conf

[ -e /tftpboot/initrd.img-2.6.8-1-386 ] || cp boot/initrd.img-2.6.8-1-386 /tftpboot
[ -e /tftpboot/vmlinuz-2.6.8-1-386 ] || cp boot/vmlinuz-2.6.8-1-386 /tftpboot

cd -

