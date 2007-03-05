#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

grep -qF /usr/pluto/lib /etc/ld.so.conf || echo "/usr/pluto/lib" >>/etc/ld.so.conf
/sbin/ldconfig
