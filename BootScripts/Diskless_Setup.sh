#!/bin/bash

RunSQL()
{
	local Q
	Q="$*"
	[ -z "$Q" ] || echo "$Q;" | mysql pluto_main | tail +2 | tr '\n\t ' ' ,~'
}

Field()
{
	local Row FieldNumber
	FieldNumber="$1"; shift
	Row="$*"
	echo "$Row" | cut -d, -f"$FieldNumber" | tr '~' ' '
}

# vars:
# CORE_INTERNAL_ADDRESS
# INTERNAL_SUBNET
# INTERNAL_SUBNET_MASK
# MOON_ENTRIES
# MOON_ADDRESS
ReplaceVars()
{
	# TODO
	local File Commands Vars VarValue SedCmd
	File="$1"
	
	Vars="CORE_INTERNAL_ADDRESS INTERNAL_SUBNET INTERNAL_SUBNET_MASK MOON_ENTRIES MOON_ADDRESS"

	for i in $Vars; do
		eval "VarValue=\"\$$i\""
		VarValue=${VarValue//\//\\\/}
		SedCmd="s/%$i%/$VarValue/g"
		[ -z "$Commands" ] && Commands="$SedCmd" || Commands="$Commands; s/%$i%/$VarValue/g"
	done
	sed -i "$Commands" $File
}

# Create Server-side files

# TODO: Core configuration in database; currently we assume we have two network cards, eth1 being the internal one and using dhcp on it (kind of utopic)
ServerConf=$(ifconfig eth0 | awk 'NR==2' | perl -ne 's/^.*inet addr:(\S+)\s+Bcast:(\S+)\s+Mask:(\S+).*$/\1,\2,\3/g; print')
CORE_INTERNAL_ADDRESS=$(Field 1 "$ServerConf")
INTERNAL_SUBNET_MASK=$(Field 3 "$ServerConf")
for i in 1 2 3 4; do
	IPDigit=$(echo $CORE_INTERNAL_ADDRESS | cut -d. -f$i)
	MaskDigit=$(echo $INTERNAL_SUBNET_MASK | cut -d. -f$i)
	NetDigit=$(($IPDigit & $MaskDigit))
	INTERNAL_SUBNET="$INTERNAL_SUBNET$Dot$NetDigit" && Dot="."
done

echo "Getting list of Media Directors"
Q="SELECT IPaddress, MACaddress
FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL AND IPaddress IS NOT NULL AND MACaddress IS NOT NULL"
R=$(RunSQL "$Q")

echo "Setting diskless MD filesystems"
for Client in $R; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")
	if ! [ -d /usr/pluto/diskless/$IP -a -e /usr/pluto/diskless/$IP/etc/diskless.conf ]; then
		echo "Creating initial filesystem for moon '$IP,$MAC'"
		/usr/pluto/bin/Create_DisklessMD_FS.sh "$IP" "$MAC"
	fi
done

echo "Setting up /etc/exports"
cp /usr/pluto/templates/exports.tmpl /etc/exports.$$
for Client in $R; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")
	echo "/usr/pluto/diskless/$IP $IP/255.255.255.255(rw,no_root_squash,no_all_squash,sync)" >>/etc/exports.$$
done
ReplaceVars /etc/exports.$$
mv /etc/exports.$$ /etc/exports
/etc/init.d/nfs-kernel-server restart

echo "Setting up /etc/dhcp3/dhcpd.conf"
cp /usr/pluto/templates/dhcpd.conf.tmpl /etc/dhcp3/dhcpd.conf.$$
MoonNumber=1
for Client in $R; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")
	MOON_ENTRIES=$(printf "%s" "$MOON_ENTRIES\n\thost moon$MoonNumber { hardware ethernet $MAC; fixed-address $IP; }")
	MoonNumber=$((MoonNumber+1))
done
ReplaceVars /etc/dhcp3/dhcpd.conf.$$
mv /etc/dhcp3/dhcpd.conf.$$ /etc/dhcp3/dhcpd.conf
/etc/init.d/dhcp3-server restart

mkdir -p /tftpboot/pxelinux.cfg
for Client in $R; do
	IP=$(Field 1 "$Client")
	HexIP=$(gethostip -x "$IP")
	MAC=$(Field 2 "$Client")
	lcdMAC=$(echo ${MAC//:/-} | tr 'A-Z' 'a-z')
	echo "Setting up /tftpboot/pxelinux.cfg/01-$lcdMAC"
	cp /usr/pluto/templates/pxelinux.tmpl /tftpboot/pxelinux.cfg/01-$lcdMAC.$$
	MOON_ADDRESS="$IP"
	ReplaceVars /tftpboot/pxelinux.cfg/01-$lcdMAC.$$
	mv /tftpboot/pxelinux.cfg/01-$lcdMAC.$$ /tftpboot/pxelinux.cfg/01-$lcdMAC
done

# Create Client-side files
MoonNumber=1
for Client in $R; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")
	echo -n "Setting moon '$IP,$MAC':"
	
	echo -n " /etc/fstab"
	cp /usr/pluto/templates/fstab.tmpl /usr/pluto/diskless/$IP/etc/fstab.$$
	ReplaceVars /usr/pluto/diskless/$IP/etc/fstab.$$
	mv /usr/pluto/diskless/$IP/etc/fstab.$$ /usr/pluto/diskless/$IP/etc/fstab
	
	echo -n " /etc/hosts"
	cp /usr/pluto/templates/hosts.tmpl /usr/pluto/diskless/$IP/etc/hosts.$$
	ReplaceVars /usr/pluto/diskless/$IP/etc/hosts.$$
	mv /usr/pluto/diskless/$IP/etc/hosts.$$ /usr/pluto/diskless/$IP/etc/hosts

	echo -n " /etc/hostname"
	echo "moon$MoonNumber" >/usr/pluto/diskless/$IP/etc/hostname

	echo -n " Startup_Scripts"
	/etc/init.d/Startup_Scripts.sh script "$IP" >/usr/pluto/diskless/$IP/etc/pluto.startup

	echo -n " MythTV DB Settings"
	mkdir -p /usr/pluto/diskless/$IP/etc/mythtv
	cp /etc/mythtv/mysql.txt /usr/pluto/diskless/$IP/etc/mythtv/mysql.txt.$$
	SedCmd="s/^DBHostName.*/DBHostName=dce_router/g"
	sed -i "$SedCmd" /usr/pluto/diskless/$IP/etc/mythtv/mysql.txt.$$
	mv /usr/pluto/diskless/$IP/etc/mythtv/mysql.txt.$$ /usr/pluto/diskless/$IP/etc/mythtv/mysql.txt

	echo -n " Start_LocalDevices"
	Q="SELECT PK_Device FROM Device WHERE IPaddress='$IP' AND MACaddress='$MAC' LIMIT 1"
	PK_Device=$(RunSQL "$Q")
	/usr/pluto/bin/Start_LocalDevices.sh script "$IP" -d "$PK_Device" >/usr/pluto/diskless/$IP/usr/pluto/bin/Start_LocalDevices.sh

	echo
	MoonNumber=$((MoonNumber+1))
done
