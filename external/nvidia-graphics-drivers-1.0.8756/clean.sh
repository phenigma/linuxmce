#!/bin/bash

Version=$(dpkg-parsechangelog -l./debian/changelog |grep ^Version|cut -d' ' -f2)
for i in $(svn stat | grep '\?' | awk '{print $2}'); do
	[[ "$i" = "NVIDIA-Linux-x86-1.0-${Version}-pkg1.run" ]] && continue
	rm -rf "$i"
done

for i in $(svn stat | awk '{print $2}'); do
	[[ "$i" = "NVIDIA-Linux-x86-1.0-${Version}-pkg1.run" ]] && continue
	svn revert "$i"
done
