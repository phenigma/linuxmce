#!/bin/bash

pushd trunk
dpkg-buildpackage -rfakeroot -b -us -uc
popd
pushd modules-2.2/modules
for dir in *; do
	pushd "$dir"
	dpkg-buildpackage -rfakeroot -b -us -uc
	popd
done
popd
