#!/bin/bash

mkdir -p /usr/lib/pluto-dhcpd-plugin
dpkg-divert --add --rename --package pluto-dhcpd-plugin --divert /usr/lib/pluto-dhcpd-plugin/dhcpd3 /usr/sbin/dhcpd3 > /dev/null
