#!/bin/bash

#$0 <MAC> <path_to_sis>

if [ "$#" -ne "2" ]; then
	echo "Usage: $0 <MAC> <path_to_sis>"
	exit
fi

MAC="$1"
SIS="$2"

#TODO: check if MAC is valid

if [ ! -f "$SIS" -o ! -r "$SIS" ]; then
	echo "sis file '$SIS' not found or not accessible"
	exit
fi

rfcomm="# RFCOMM configuration file.
#
# $Id: PhoneInstall.sh,v 1.1 2004/10/18 08:00:15 radu Exp $
#

rfcomm0 {
    # bluetooth address of the device
    device $MAC;

    # RFCOMM channel for the connection
    channel 9;

    # Description of the connection
    comment \"Nokia 3650 OBEX push\";
}"

echo "$rfcomm" >/etc/bluetooth/rfcomm.conf
/etc/init.d/bluez-utils restart
rfcomm bind /dev/rfcomm0 "$MAC" 9 &&
rfcomm connect rfcomm0 "$MAC" &&
ussp-push /dev/rfcomm0 "$SIS" PlutoMO.sis

