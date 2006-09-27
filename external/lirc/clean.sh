#!/bin/bash

Version=$(dpkg-parsechangelog -l./debian/changelog |grep ^Version|cut -d' ' -f2|cut -d. -f3|cut -d- -f1)
for i in $(svn stat --no-ignore | egrep '^\?|^I' | awk '{print $2}'); do
	rm -rf "$i"
done

svn revert -R .
