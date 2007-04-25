#!/bin/bash

apt-get -y install libdvdread3 || exit 1
/usr/share/doc/libdvdread3/install-css.sh || exit 2
exit 0
