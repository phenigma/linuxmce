if ! grep -q "evdev" /etc/modules; then
/usr/pluto/bin/Debug_LogKernelModules.sh "$0"
	echo "evdev" >> /etc/modules
fi

/sbin/modprobe evdev

if [[ -x /etc/init.d/udev ]] ;then
	invoke-rc.d udev force-reload
fi

