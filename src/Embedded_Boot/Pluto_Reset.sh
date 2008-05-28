#!/bin/sh

mkdir -p /etc/pluto/save/
cp /etc/pluto/* /etc/pluto/save/
cp /etc/pluto/orig/* /etc/pluto/
reboot
