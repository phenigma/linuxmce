#!/bin/bash
mkdir -p /home/mirrors/replacements/main/binary-i386
cd /home/mirrors/replacements/main/binary-i386
rm -f asterisk_1.0.7.dfsg.1.pluto.2-2_i386.deb
rm -f fglrx-driver-dev_8.16.20-1_i386.deb
rm -f kernel-headers-2.6.10-1-686_2.6.10-4_i386.deb
rm -f libasound2_1.0.8-1_i386.deb
rm -f libc6-dev_2.3.2.ds1-21_i386.deb
rm -f libfontconfig1-dev_2.3.1-2_i386.deb
rm -f libqt3c102-mt_3.3.4-3_i386.deb
rm -f libqt3c102-mysql_3.3.4-3_i386.deb
rm -f lirc-2.6.10-1-686_0.7.1+0.7.0-2_i386.deb
rm -f Packages.gz
rm -f pluto-liblinphone-dev_0.12.2-2_i386.deb
tar -xzvf /home/uploads/replacements.tar.gz
