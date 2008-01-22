#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
set -x

DisplayMessage "*** STEP: Download closed source debs"
trap 'Error "Undefined error in $0"' EXIT

DisplayMessage "Getting latest ID"
LatestID=$(2>/dev/null wget -O - "$closed_source_debs_url/latest")
mkdir -p "$out_dir"/tmp
pushd "$out_dir"/tmp
	DisplayMessage "Downloading latest archive"
	wget -O "$LatestID".tar "$closed_source_debs_url/$LatestID.tar"
	DisplayMessage "Extracting archive"
	tar -xvf "$LatestID".tar
	DisplayMessage "Removing archive"
	rm -f "$LatestID".tar
popd

trap - EXIT
