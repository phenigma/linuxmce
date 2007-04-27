#!/bin/bash

set -e 

. ./filesd-parms.sh $@

File="/etc/init.d/mountnfs.sh"
Content=""

Content="${Content}#!/bin/bash\n"
Content="${Content}mount -a -t nfs,nfs4,smbfs,cifs,ncp,ncpfs,coda,ocfs2,gfs\n"

mkdir -p "${Parm_RootLocation}/$(dirname $File)"
echo -e "${Content}" > "${Parm_RootLocation}/${File}"
chmod +x "${Parm_RootLocation}/${File}"

Symlink="${Parm_RootLocation}/etc/rcS.d/S44mountnfs.sh"
mkdir -p "${Parm_RootLocation}/etc/rcS.d"
rm -f "${Symlink}"
ln -s "$File" "${Symlink}" 2>/dev/null || /bin/true

## This script is broken so we better remove it
rm -f ${Parm_RootLocation}/etc/init.d/waitnfs.sh
