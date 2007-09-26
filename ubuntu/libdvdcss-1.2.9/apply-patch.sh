#!/bin/bash
version=1.2.9
tar zxvf "libdvdcss-$version.tar.gz"
cd "libdvdcss-$version"
patch -p0 < ../fix-uninitialized-disk-key.diff
cd ..
tar -czf "libdvdcss-$version.tar.gz" "libdvdcss-$version"
rm -rf "libdvdcss-$version"
echo "Patched"
