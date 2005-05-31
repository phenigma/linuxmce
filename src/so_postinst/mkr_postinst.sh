#!/bin/bash

grep -qF /usr/pluto/lib /etc/ld.so.conf || echo "/usr/pluto/lib" >>/etc/ld.so.conf
/sbin/ldconfig
