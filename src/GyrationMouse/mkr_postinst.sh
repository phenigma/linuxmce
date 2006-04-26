if ! grep -q "evdev" /etc/modules; then
	echo "evdev" >> /etc/modules
fi

/sbin/modprobe evdev

if [[ -x /etc/init.d/udev ]] ;then
	/etc/init.d/udev force-reload
fi

