#!/bin/bash

set -e

echo ""
echo "********************************************************************************"
echo "*** Running: $0"
echo "********************************************************************************"

rsync_skin_host="10.0.2.3"
rsync_media_host="10.0.2.3"

http_skin_host="www.linuxmce.org"
http_media_host="www.linuxmce.org"

# needs a better name
base_dir=/home/samba/www_docs

. /etc/lmce-build/builder.conf

function ImportPublicSkinFromRSyncHost {
	local skins_dir=${base_dir}/graphics
# we don't want this on a lmce machine
#	mkdir -p /usr/pluto/orbiter/
	mkdir -p "$skins_dir"

# we don't want this on a lmce machine
# get MakeRelease to take them from "$skins_dir" instead
#	rm -f /usr/pluto/orbiter/skins
#	ln -s "$skins_dir" /usr/pluto/orbiter/skins

	apt-get -y install rsync
	pushd "$skins_dir"
		rsync -avz --delete-after rsync://$rsync_skin_host/skins-public ./
	popd

# we don't want this on a lmce machine
# get MakeRelease to take them from "$skins_dir" instead
#	pushd /usr/pluto/orbiter/skins
	pushd "$skins_dir"
		cp -r LinuxMCE/* Basic/
	popd

	pushd "${build_dir}"
		rm -f samba
		ln -s /home/samba
	popd
}

function ImportPublicMediaFromRSyncHost {
	mkdir -p "${base_dir}/sample media/"
	pushd "${base_dir}/sample media/"
		rsync -avz --delete-after rsync://$rsync_skin_host/sample-media ./
	popd
}


function DownloadIfNeeded {
	local host=$1
	local filepath=$2
	local filename=$3

	local md5_cur="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"


	mkdir -p "$filepath"
	pushd "$filepath"

		# move current md5sum file out of the way
		if [ -e "${filename}.md5.txt" ]
		then
			mv -f "${filename}.md5.txt" "${filename}.last.md5.txt"
		fi

		# get current md5sum from file
		if [ -e "${filename}.last.md5.txt" ]
		then
			md5_cur=$(cat "${filename}.last.md5.txt")
		fi

		# get new md5sum file
	     	wget http://$host/build/${filename}.md5.txt

		# get new md5sum from file
		local md5_new=$(cat "${filename}.md5.txt")

		if [ "$md5_cur" != "$md5_new" ]
		then
			
			# move old file out of the way
			mv -f "${filename}" "${filename}.last"

			# download new file
			wget http://$host/build/${filename}

			# calculate it's md5sum
			md5_cur=$(md5sum ${filename})

			if [ "$md5_cur" = "$md5_new" ]
			then

				# remove old files
				rm -f "${filename}.last" "${filename}.last.md5.txt"

			else

				rm -f "${filename}" "${filename}.md5.txt"

				# restore old files if they exist
				if [ -e "${filename}.last" ]
				then
					mv -f "${filename}.last" "${filename}"
					mv -f "${filename}.last.md5.txt" "${filename}.md5.txt"
				fi

			fi

		fi

	popd

	return $(test "$md5_cur" = "$md5_new")
}


function ImportPublicSkinFromHTTPHost {
	local skins_dir="${base_dir}/graphics"

	if DownloadIfNeeded $http_skin_host "${base_dir}" "graphics.tbz" ; then
		pushd "${base_dir}"
			rm -rf "graphics"
			tar xvfj "graphics.tbz"
		popd

# we don't want this on a lmce machine
# get MakeRelease to take them from "$skins_dir" instead
#		mkdir -p /usr/pluto/orbiter/
#		ln -s "/home/samba/www_docs/graphics" /usr/pluto/orbiter/skins
#		pushd /usr/pluto/orbiter/skins
		pushd "${skins_dir}"
			cp -r LinuxMCE/* Basic/
		popd

		pushd "${build_dir}"
			rm -f samba
			ln -s /home/samba samba
		popd
	fi
}


function ImportPublicMediaFromHTTPHost {
	if DownloadIfNeeded $http_media_host "${base_dir}" "sample_media.tbz" ; then
		pushd "${base_dir}"
			rm -rf "sample media"
			tar xvfj "sample_media.tbz"
		popd
	fi
}

function ImportWizardVideosFromHTTPHost {
	if DownloadIfNeeded "$http_media_host" "/var/lmce-build/extras/" video-wizard-videos.tar ; then
		pushd /var/lmce-build/extras/
			rm -rf video-wizard-videos
			tar xf video-wizard-videos.tar
			pushd video-wizard-videos
				dpkg-deb -b . ..
			popd
		popd
	fi
}

function IsReachable {
	local host=$1
	echo "Checking if $2 ($host) is reachable"
	local tmp=`ping -c 5 -i 0.2 $host  | grep '[^0] received'` || true
	if [ x"$tmp" != x"" ] ; then
	    return $(true)
	fi
	return $(false)
}

# Import skin
if IsReachable "$rsync_skin_host" "rsync skin host" ; then
	ImportPublicSkinFromRSyncHost
elif IsReachable "$http_skin_host" "http skin host" ; then
	ImportPublicSkinFromHTTPHost
else
	echo "Failed to download skins."
	echo "If you don't have them already, cd2-build.sh will fail."
fi

# Import media
if IsReachable "$rsync_media_host" "rsync media host" ; then
	ImportPublicMediaFromRSyncHost
elif IsReachable "$http_media_host" "http media host" ; then
	ImportPublicMediaFromHTTPHost
else
	echo "Failed to download media."
	echo "If you don't have them already, cd2-build.sh will fail."
fi

# Import videos
if IsReachable "$http_media_host" "http media host" ; then
	ImportWizardVideosFromHTTPHost
fi

echo "*** Done: $0"

