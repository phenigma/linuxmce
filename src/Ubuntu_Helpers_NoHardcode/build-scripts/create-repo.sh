#!/bin/bash

. /etc/lmce-build/builder.conf

function MoveDebs2Repo {
	# Create the repository dir
	mkdir -p "$local_mirror_dir"

	# Remove any existing debs and Packages files
	rm -f "$local_mirror_dir/*.deb"
	rm -f "$local_mirror_dir/Packages*"

	# Copy the debs built by make-replease
	cp "${out_dir}/tmp/*.deb" "$local_mirror_dir"

	# Copy the debs from replacements
	cp "${replacements_dir}/*.deb" "$local_mirror_dir"

	# Generate the Packages files
	pushd "$local_mirror_dir"
		dpkg-scanpackages . /dev/null > Packages
		cat Packages | gzip -9c > Packages.gz
	popd
}

MoveDebs2Repo
