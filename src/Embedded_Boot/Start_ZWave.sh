#!/bin/sh

while [[ ! -f /var/run/pluto.stop ]]; do
	/usr/bin/ZWave -r 127.0.0.1 -d 1
done
