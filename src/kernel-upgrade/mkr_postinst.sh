#!/bin/bash

PrevVer="$2"

echo "Setting debconf front-end to Noninteractive"
awk '/Name: debconf\/frontend/,/^$/ {if ($1 == "Value:") print "Value: noninteractive"; else print; next}
	{print}' /var/cache/debconf/config.dat > /var/cache/debconf/config.dat.$$
mv /var/cache/debconf/config.dat.$$ /var/cache/debconf/config.dat

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
