#!/bin/bash

NoSpace="s/ //g"

sed "$NoSpace" /etc/pluto.conf >/tmp/pluto.conf
while read line; do
	eval "$line"
done </tmp/pluto.conf
