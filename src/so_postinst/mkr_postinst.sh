#!/bin/bash

grep -qF /usr/pluto/bin /etc/ld.so.conf || echo "/usr/pluto/bin" >>/etc/ld.so.conf
/sbin/ldconfig
