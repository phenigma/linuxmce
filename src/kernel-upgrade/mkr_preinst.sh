#!/bin/bash

if ! grep -q "silent_modules" /etc/kernel-img.conf; then
	# should we write the entire file just to be sure?
	echo "silent_modules = yes" >>/etc/kernel-img.conf
fi
