#!/bin/bash

if [ "$#" -ne 4 ]; then
	echo "Syntax: $0 <IP> <MAC> <Device> <Activation Code>"
	exit 1
fi

IP="$1"
MAC="$2"
Device="$3"
Code="$4"

FSarchive=PlutoMD.tar.bz2
HexIP=$(gethostip -x "$IP")

echo "Extracting filesystem for diskless client '$IP , $MAC' ($Device)"

mkdir -p /usr/pluto/diskless/"$IP"
cd /usr/pluto/diskless/"$IP"
tar -xjvf /usr/pluto/install/"$FSarchive" >/dev/null
conf="IP=$IP
MAC=$MAC
Device=$Device
Code=$Code"

echo "$conf" >etc/diskless.conf

[ -e /tftpboot/initrd.img-2.6.8-1-686 ] || cp boot/initrd.img-2.6.8-1-686 /tftpboot
[ -e /tftpboot/vmlinuz-2.6.8-1-686 ] || cp boot/vmlinuz-2.6.8-1-686 /tftpboot

cd -

