#!/bin/bash

if [[ -f /etc/init.d/kdm ]]; then
	update-rc.d check_avwizard start 98 2 . >/dev/null
	update-rc.d 0start_avwizard start 99 2 . >/dev/null
fi
