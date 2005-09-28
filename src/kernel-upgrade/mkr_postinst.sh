#!/bin/bash

PrevVer="$2"

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
