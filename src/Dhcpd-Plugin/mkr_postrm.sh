#!/bin/bash

dpkg-divert --remove --rename --package pluto-dhcpd-plugin --divert /usr/lib/pluto-dhcpd-plugin/dhcp3-server.initd /etc/init.d/dhcp3-server > /dev/null
