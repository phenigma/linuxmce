#!/bin/bash
# setup NFS exports

. /usr/pluto/bin/pluto.func

RunSQL()
{
	local Q
	Q="$*"
	echo "$Q;" | /usr/bin/mysql pluto_main | tail +2 | tr '\t\n' ', '
}

Q="SELECT PK_Device,IPaddress,MACaddress FROM Device WHERE FK_DeviceTemplate=28"
R=$(RunSQL "$Q")

mkdir -p /usr/pluto/workstations
cat /usr/pluto/templates/exports.tmpl | perl -pe "s/192\.168\.2/$network/g"> /etc/exports.$$

for MD in $R; do
	PK_Device=$(echo "$MD" | cut -d, -f1)
	IPaddress=$(echo "$MD" | cut -d, -f2)
	MACaddress=$(echo "$MD" | cut -d, -f3)

	ExportsEntry="/usr/pluto/workstations/$IPaddress $IPaddress/255.255.255.255(rw,no_root_squash,no_all_squash)"
	echo "$ExportsEntry" >>/etc/exports.$$
done

mv -f /etc/exports.$$ /etc/exports
exportfs -ar &>/dev/null &
