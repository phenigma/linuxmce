#!/bin/bash

awk '/Section..*"Files"/,/EndSection/ { if ($1 == "ModulePath") print "\tModulePath\t\"/usr/lib/xorg/modules\""; else print; next } { print }' /etc/X11/xorg.conf >/etc/X11/xorg.conf.$$
mv /etc/X11/xorg.conf{.$$,}
