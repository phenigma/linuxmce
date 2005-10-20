#!/bin/bash

if [ ! -f /etc/init.d/vdrdevel ]; then
	sed 's/DAEMON=\/usr\/sbin\/runvdrdevel/DAEMON=\/usr\/sbin\/runvdrdevelserver/g' /etc/init.d/vdrdevel > /etc/init.d/vdrdevel.tmp
	sed 's/PIDFILE=\/var\/run\/runvdrdevel.pid/PIDFILE=\/var\/run\/runvdrdevelserver.pid/g' /etc/init.d/vdrdevel.tmp > /etc/init.d/vdrdevel
	rm /etc/init.d/vdrdevel.tmp
fi
