#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
set -x

function MoveDebs2Repo {
	# Create the repository dir
	mkdir -p "$local_mirror_dir"

	# Remove any existing debs and Packages files
	DisplayMessage "Removing old debs from repository"
	rm -f "$local_mirror_dir"/*.deb
	rm -f "$local_mirror_dir"/Packages*

	# Copy the debs built by make-replease
	DisplayMessage "Copying make-realase debs to repository"
	cp "${out_dir}/tmp/"*.deb "$local_mirror_dir"

	# Copy the debs from replacements
	DisplayMessage "Copying replacements debs to repository"
	cp "${replacements_dir}"/*.deb "$local_mirror_dir"

	# Copy the debs from extra
	DisplayMessage "Copying extra (downloaded) debs to repository"
	cp "${extras_dir}"/*.deb "$local_mirror_dir"

	# Generate the Packages files
	DisplayMessage "Generating Packages / Packages.gz files"
	pushd "$local_mirror_dir"
		dpkg-scanpackages . /dev/null > Packages
		cat Packages | gzip -9c > Packages.gz
	popd
}

function CopyDebsToDisklessSync {
	local sync_pkgs="pluto-bluetooth-dongle pluto-cm11a pluto-eib pluto-gc100 pluto-generic-serial-device pluto-hdhomerun pluto-irbase pluto-irtrans-wrapper pluto-libbd pluto-lirc-wrapper pluto-messagetrans pluto-motion-wrapper pluto-msiml-disp-butt pluto-nvidia-video-drivers pluto-powerfile-c200 pluto-proxy-orbiter pluto-text-to-speech pluto-tira-wrapper pluto-usb-uirt-0038 pluto-vdr pluto-vipshared pluto-wavetrend-reader pluto-zwave-lighting mtx-pluto nvidia-glx lirc-pluto lirc-modules-2.6.22-14-generic id-my-disc linux-restricted-modules-2.6.22-14-generic lmcevdr pluto-vdr-plugin pluto-vdr vdr-dev vdr-plugin-control vdr-plugin-xineliboutput vdr libxine-xvdr libxineliboutput-sxfe xineliboutput-sxfe mythtv-backend mythtv-common mythtv-frontend mythtv-transcode-utils"
	local pkg
	mkdir -p "${build_dir}/DisklessSync/${arch}/deb-cache"
	for pkg in $sync_pkgs ;do
		rm -f "${build_dir}/DisklessSync/${arch}/deb-cache/${pkg}_"*.deb
		cp "/var/www/${pkg}_"*.deb "${build_dir}/DisklessSync/${arch}/deb-cache/"
	done
	rm -f "${build_dir}/DisklessSync/${arch}/deb-cache/3m-touchware_"*.deb
	cp /var/www/3m-touchware_*.deb "${build_dir}/DisklessSync/${arch}/deb-cache" || :
}

function PublishPrivateDebs {
        # copy closed source debs
        local ID=$(date +%Y%m%d%H%M%S)
        local priv_debs=(fiire-drivers pluto-wavetrend-reader pluto-zwave-lighting)
        local pkg

	local temp_dir=$(mktemp -d)
        for pkg in "${priv_debs[@]}"; do
                cp "$out_dir"/tmp/"$pkg"_*.deb "$temp_dir"
        done

        local Dir="$local_mirror_dir/priv_debs"
        local Latest="$Dir/latest"

        mkdir -p "$Dir"
        tar -C "$temp_dir" -cvf "$Dir"/"$ID".tar .
        rm -f "$Latest"
        echo "$ID" >"$Latest"

	rm -rf "$temp_dir"
}

DisplayMessage "*** STEP: Creating local repository"
trap 'Error "Undefined error in $0"' EXIT
MoveDebs2Repo

DisplayMessage "*** STEP: Copy debs to diskless common directory (for dvd)"
CopyDebsToDisklessSync

if [[ "$svn_private_url" != "" ]] && [[ "$svn_private_user" != "" ]] && [[ "$svn_private_pass" != "" ]] ;then
	DisplayMessage "*** STEP: Publish private debs"
	PublishPrivateDebs
fi

trap - EXIT

