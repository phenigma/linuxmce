#!/bin/bash

rm /usr/sbin/dhcpd3 || :
dpkg-divert --remove --rename --package pluto-dhcpd-plugin --divert /usr/lib/pluto-dhcpd-plugin/dhcpd3 /usr/sbin/dhcpd3 > /dev/null
