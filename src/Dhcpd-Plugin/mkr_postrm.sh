#!/bin/bash

dpkg-divert --remove --rename --package pluto-dhcpd-plugin --divert /usr/lib/pluto-dhcpd-plugin/dhcpd3 /usr/sbin/dhcpd3 > /dev/null
