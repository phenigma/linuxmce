#!/bin/bash

IP="$1"
MAC="$2"
lcdMAC=$(echo ${MAC//:/-} | tr 'A-Z' 'a-z')

echo "Deleting MD files. MAC=$MAC IP=$IP"

echo "Filesystem"
rm -rf /home/diskless/"$IP"

echo "TFTP"
rm -rf /tftpboot/"$IP"
rm -rf /tftpboot/pxelinux.cfg/"01-$lcdMAC"
