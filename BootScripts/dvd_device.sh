#!/bin/bash

[ -e /dev/cdrom ] && cp /dev/cdrom /dev/dvd || echo "Debian didn't make a cdrom link. Do you have one?"

