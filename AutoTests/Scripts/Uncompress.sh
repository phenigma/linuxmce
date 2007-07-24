#!/bin/bash

#cp /tmp/AutoTests/newhb/newhb_bin_* /usr/pluto/bin
#cp /tmp/AutoTests/newhb/newhb_lib_* /usr/pluto/lib
echo "Start uncompressing" > /uncompress.log
cd /usr/pluto/bin
tar jxf /tmp/AutoTests/newhb/newhb_bin_*

cd /usr/pluto/lib
tar jxf /tmp/AutoTests/newhb/newhb_lib_*

for md in /usr/pluto/diskless/* ;do
	if [[ -e "$md"'/usr/pluto/bin' ]]; then
		cd  "$md"'/usr/pluto/bin'
		tar jxf /tmp/AutoTests/newhb/newhb_bin_*
	fi


	if [[ -e "$md"'/usr/pluto/lib' ]]; then
		cd "$md"'/usr/pluto/lib'
		tar jxf /tmp/AutoTests/newhb/newhb_lib_*
	fi
done

echo "Uncompress finished" >> /uncompress.log
	
