#!/bin/bash

for i in $(svn stat | grep '\?' | awk '{print $2}'); do
	[[ "$i" = "NVIDIA-Linux-x86-1.0-8178-pkg1.run" ]] && continue
	rm -rf "$i"
done

for i in $(svn stat | awk '{print $2}'); do
	[[ "$i" = "NVIDIA-Linux-x86-1.0-8178-pkg1.run" ]] && continue
	svn revert "$i"
done
