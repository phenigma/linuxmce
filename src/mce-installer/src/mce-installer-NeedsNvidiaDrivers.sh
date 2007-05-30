#!/bin/bash

echo -n "Searching for nvidia in xorg.conf ... "
if grep -iq "Driver.*\"nvidia\"" /etc/X11/xorg.conf ;then
	echo "found"
	exit 1
fi

echo -n "Searching for nv in xorg.conf ... "
if grep -iq "Driver.*\"nv\"" /etc/X11/xorg.conf ;then
	echo "found"
	exit 0
fi
echo "not found"

echo -n "Searching for nvidia card in lspci ... "
if lspci | grep -iq "VGA .*nvidia" ;then
	echo "found"
	exit 0
fi
echo "not found"

exit 1
