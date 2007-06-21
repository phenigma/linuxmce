#!/bin/bash

. /tmp/mce_wizard_data.sh
. ./mce-installer-common.sh

Setup_Apt_Conffiles

apt-get -y install lame || exit 1

exit 0
