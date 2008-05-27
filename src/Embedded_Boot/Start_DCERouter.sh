#!/bin/sh

while [[ ! -f /var/run/pluto.stop ]]; do
	/usr/bin/DCERouter
done
