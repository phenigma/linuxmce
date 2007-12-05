#!/bin/bash

. /etc/lmce-build/builder.conf

#TODO:
# Kill kirill cause i need to copy mysql_wrapper in /usr/pluto/bin
# Implement a system that compiles the bins only when they're changed (similar to replacements)

function Build_MakeRelease {
	DisplayMessage "**** STEP : PREPARING BUILD SYSTEM (MakeRelase)"

	DisplayMessage "Precompiling pluto_main"
	pushd "${svn_dir}/trunk/src/pluto_main"
	make || Error "Failed to precompile pluto_main to use for MakeRelease"
	popd

	DisplayMessage "Precompiling PlutoUtils"
	pushd "${svn_dir}/trunk/src/PlutoUtils"
	make || Error "Failed to precompile PlutoUtils"
	popd

	DisplayMessage "Precompiling SerializeClass"
	pushd "${svn_dir}/trunk/src/SerializeClass"
	SNR_CPPFLAGS="" make || Error "Failed to precompile SerializeClass"
	popd

	DisplayMessage "Precompiling LibDCE"
	pushd "${svn_dir}/trunk/src/DCE"
	SNR_CPPFLAGS="" make || Error "Failed to precompile LibDCE"
	popd

	DisplayMessage "Precompiling MakeRelease"
	pushd "${svn_dir}/trunk/src/MakeRelease"
	make -f Makefile.MakeRelease || Error "Failed to precompile MakeRelease"
	popd

	DisplayMessage "Precompiling MakeRelease_PrepFiles"
	pushd "${svn_dir}/trunk/src/MakeRelease_PrepFiles"
	make || Error "Failed to precompile MakeRelease_PrepFiles"
	popd

	DisplayMessage "Precompiling mysql_wrapper"
	pushd "${svn_dir}/trunk/src/mysql_wrapper"
	make || Error "Failed to precompile mysql_wrapper"
	popd

	mkdir -p "${mkr_dir}"
	mkdir -p /usr/pluto/bin
	cp "${svn_dir}/trunk/src/bin/MakeRelease" "${mkr_dir}"
	cp "${svn_dir}/trunk/src/bin/MakeRelease_PrepFiles" "${mkr_dir}"
	cp "${svn_dir}/trunk/src/bin/mysql_wrapper" /usr/pluto/bin/
	cp "${svn_dir}/trunk/src/lib/*.so" "${mkr_dir}"
}

Build_MakeRelease
