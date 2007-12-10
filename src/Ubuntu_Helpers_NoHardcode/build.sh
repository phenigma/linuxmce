#!/bin/bash
build_scripts_dir="/usr/local/lmce-build/build-scripts"

"${build_scripts_dir}/checkout-svn.sh"
"${build_scripts_dir}/import-win32-bins.sh"
"${build_scripts_dir}/import-skins.sh"

"${build_scripts_dir}/build-makerelease.sh"
"${build_scripts_dir}/build-replacements.sh"

"${build_scripts_dir}create-repo.sha"

"${build_scripts_dir}cd1-build.sh"
"${build_scripts_dir}cd2-build.sh"
