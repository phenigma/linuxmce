#!/bin/bash

if [ "$1" = "remove" ]; then
	dpkg-divert --remove /sbin/lrm-video
fi
