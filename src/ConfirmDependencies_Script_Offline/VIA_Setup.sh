#!/bin/bash

# add pci=conf1 to VIA kernel in GRUB
if ! grep -q "^# kopt=.* pci=conf1" /boot/grub/menu.lst; then
	oldKopt=$(grep "^# kopt=.*" /boot/grub/menu.lst)
	newKopt="$oldKopt pci=conf1"
	sed -i "s~$oldKopt~$newKopt~g" /boot/grub/menu.lst
	/usr/sbin/update-grub
fi
