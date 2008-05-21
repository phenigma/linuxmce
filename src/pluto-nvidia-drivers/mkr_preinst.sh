#!/bin/bash

if [[ "$1" == "install" ]] ;then
	dpkg-divert --add --rename --package pluto-nvidia-drivers --divert /sbin/lrm-video.pluto /sbin/lrm-video
fi
