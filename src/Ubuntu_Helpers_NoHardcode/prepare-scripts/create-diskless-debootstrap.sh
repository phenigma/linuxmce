#!/bin/bash

#TODO: Move stuff from /home/DisklessFS to some other standar place, like /var/lmce-build/something. 
# This will also require a change in the database.

function Create_Diskless_Debootstrap_Archive {
	# Create a temp dir to store a diskless debootstrap image into
	local temp_dir="$(mktemp -d)"

	# Run debootstrap in that dir
	debootstrap gutsy "$temp_dir" http://ro.archive.ubuntu.com/ubuntu/

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

Create_Diskless_Debootstrap_Archive
