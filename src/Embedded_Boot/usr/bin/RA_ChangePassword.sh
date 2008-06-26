#!/bin/ash

RaPass=$(cat /etc/pluto/ra_password 2>/dev/null)

if [[ -n "$RaPass" ]]; then
	Pass=$(cryptpw "$RaPass")
	if ! grep -q '^remote:' /etc/passwd; then
		# add user "remote" w/ the password specified (var value)
		echo "remote:$Pass:0:0:Remote Assistance,,,:/root:/bin/ash" >>/etc/passwd
	else
		sed -i '/^remote:/ s!^\([^:]*\):[^:]*:\(.*\)$!\1:'"$Pass"':\2!' /etc/passwd
	fi
else
	if grep -q remote /etc/passwd; then
		# delete user "remote" if it exists
		sed -ri '/^remote:/d' /etc/passwd
	fi
fi
