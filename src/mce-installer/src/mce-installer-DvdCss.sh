#!/bin/bash

. /tmp/mce_wizard_data.sh
. ./mce-installer-common.h

Setup_Apt_Conffiles

apt-get -y install libdvdread3 || exit 1
/usr/share/doc/libdvdread3/install-css.sh || exit 2
exit 0
