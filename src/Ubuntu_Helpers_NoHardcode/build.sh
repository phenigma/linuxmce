#!/bin/bash -e
build_scripts_dir="/usr/local/lmce-build/build-scripts"

"${build_scripts_dir}/checkout-svn.sh"
"${build_scripts_dir}/import-win32bins.sh"

"${build_scripts_dir}/build-replacements.sh"
"${build_scripts_dir}/build-makerelease.sh"
"${build_scripts_dir}/import-databases.sh"

"${build_scripts_dir}/build-maindebs.sh"

"${build_scripts_dir}/create-repo.sh"

"${build_scripts_dir}/cd1-build.sh"
"${build_scripts_dir}/cd2-build.sh"

pushd /usr/local/lmce-build/vmware-install/
	/usr/local/lmce-build/vmware-install/install_in_vmware.sh
popd 

pushd /usr/local/lmce-build/build-dvd
	/usr/local/lmce-build/build-dvd/build-cd.sh
popd
