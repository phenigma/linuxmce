#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/env.sh

function Create_Diskless_Debootstrap_Archive {
	# Create a temp dir to store a diskless debootstrap image into
	local temp_dir="$(mktemp -d)"

	# Run debootstrap in that dir
	debootstrap "${build_name}" "$temp_dir" http://ro.archive.ubuntu.com/ubuntu/

	# Clean it's apt cache so we don't end up with it in the archive
	rm -f $temp_dir/var/cache/apt/archives/*.deb

	# Pack the diskless image in the right dir
	mkdir -p "$diskless_dir"
	pushd "$temp_dir"
		tar -jcf "$diskless_dir/PlutoMD_Debootstraped.tar.bz2" *
	popd
	
	# Remove the temp dir
	rm -rf "$temp_dir"	
}


function Create_Diskless_TFTPBoot_Package {

	#Package: pluto-default-tftpboot
	dir_="${svn_dir}/${svn_branch_name}/src/pluto-default-tftpboot-2.0.0.44.0804"
#	DisplayMessage "Building pluto-default-tftpboot"
	pushd "$dir_"
		dpkg-buildpackage -rfakeroot -us -uc -b -tc
		cp -v ../pluto-default-tftpboot_*.deb ${replacements_dir}
	popd


}


Create_Diskless_Debootstrap_Archive

Create_Diskless_TFTPBoot_Package



