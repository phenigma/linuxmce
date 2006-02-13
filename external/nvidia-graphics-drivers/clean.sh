#!/bin/bash

for i in $(svn stat | grep '\?' | awk '{print $2}'); do
	rm -rf "$i"
done

for i in $(svn stat | awk '{print $2}'); do
	svn revert "$i"
done
