#!/bin/bash
. /etc/lmce-build/builder.conf

rm -rf "${build_dir}/cd1-packages"
mkdir -p "${build_dir}/cd1-packages"

pushd ${build_dir}/cd1-packages
	aptitude download `cat /root/Ubuntu_Helpers/cd1-packages-whitelist.gutsy`
popd

