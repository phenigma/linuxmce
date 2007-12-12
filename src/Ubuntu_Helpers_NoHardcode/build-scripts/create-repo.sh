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
	rm -f "$local_mirror_dir/*.deb"
	rm -f "$local_mirror_dir/Packages*"

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


DisplayMessage "*** STEP: Creating local repository"
trap 'Error "Undefined error in $0"' EXIT

MoveDebs2Repo

trap - EXIT

