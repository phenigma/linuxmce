#!/bin/bash

. /tmp/mce_wizard_data.sh
. ./mce-installer-common.sh

Setup_Apt_Conffiles

apt-get -y install libdvdread3 || exit 1
apt-get -y install libdvdcss2 
/usr/share/doc/libdvdread3/install-css.sh
exit 0
