#!/bin/bash

remove_duplicate_debs() {
	Dir="$1"

	pushd "$Dir"
	for deb in *.deb ;do
		pkg=$(echo $deb | cut -d'_' -f1)
		ver=$(echo $deb | cut -d'_' -f2)

		for dup_deb in ${pkg}_*.deb ;do
			dup_ver=$(echo $dup_deb | cut -d'_' -f2)

			if dpkg --compare-versions "$dup_ver" lt "$ver" ;then
				echo "Remove duplicate package $pkg $dup_ver"
				rm -f "${pkg}_${dup_ver}_"*.deb
			fi

			if dpkg --compare-versions "$dup_ver" gt "$ver" ;then
				echo "Remove duplicate package $pkg $ver"
				rm -f "${pkg}_${ver}_"*.deb
				ver=$dup_ver
			fi
		done
	done
	popd
}
