#!/bin/bash


function Install {
	package_name="$1"
	package_installed="false"

	if [[ "$(aptitude show ${package_name} | grep State | cut -d ' ' -f2)" == "installed" ]] ;then
	        package_installed="true"
	fi

	if [[ "$package_installed" == "false" ]] ;then
	        dpkg -i /media/cdrom/cachecd1-cache/${package_name}_*.deb && package_installed="true"
	fi

	if [[ "$package_installed" == "false" ]] ;then
		apt-get -f -y install "${package_name}" && package_installed="true"
	fi
}

mount /dev/cdrom /media/cdrom || :
apt-get update

Install "libcucul0"
Install "libcaca0"
Install "libmad0"
Install "libxine1-ffmpeg"
Install "xine-ui"

# Prevent xine to open the config window on the first run 
for dir in /home/* /root ;do
	if [[ -d $dir/Desktop ]] ;then
		mkdir -p $dir/.xine
		touch $dir/.xine/config
	fi
done
