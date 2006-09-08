#!/bin/bash

Version=$(dpkg-parsechangelog -l./debian/changelog |grep ^Version|cut -d' ' -f2|cut -d. -f3|cut -d- -f1)
for i in $(svn stat | grep '\?' | awk '{print $2}'); do
	rm -rf "$i"
done

for i in $(svn stat | awk '{print $2}'); do
	svn revert "$i"
done
