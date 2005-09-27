#!/bin/bash

PrevVer="$2"

if ! grep -q "silent_modules" /etc/kernel-img.conf; then
	# should we write the entire file just to be sure?
	echo "silent_modules = yes" >>/etc/kernel-img.conf
fi

echo "Copying kernel package(s) for later use"
cp -v /var/cache/apt/archives/kernel-image* /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ 2>/dev/null || /bin/true

# Hack/workaround
if [[ -n "$PrevVer" ]]; then
	File=/etc/rc2.d/S01pluto_docleanup.sh
	Postinst=/var/lib/dpkg/info/pluto-kernel-upgrade.postinst

	echo '#!/bin/bash
	# Harmless, and it cleans up the dpkg journal too
	dpkg --forget-old-unavail
	rm -f "'"$File"'"' >"$File"

	chmod +x "$File"
	echo '#!/bin/bash' > "$Postinst"
	chmod +x "$Postinst"
	reboot -f
fi
