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

InRange()
{
	local Static Dynamic StaticDigit1 StaticDigit2 DynamicDigit1 DynamicDigit2
	local Digit DHCPsetting

	Digit="$1"
	DHCPsetting="$2"

	Static=$(Field 1 "$DHCPsetting")
	Dynamic=$(Field 2 "$DHCPsetting")

	StaticDigit1=$(echo "$Static" | cut -d- -f1 | cut -d. -f4)
	StaticDigit2=$(echo "$Static" | cut -d- -f2 | cut -d. -f4)
	DynamicDigit1=$(echo "$Dynamic" | cut -d- -f1 | cut -d. -f4)
	DynamicDigit2=$(echo "$Dynamic" | cut -d- -f2 | cut -d. -f4)

	[ "$Digit" -ge "$StaticDigit1" -a "$Digit" -le "$StaticDigit2" ]
	return "$?"
}

GetFreeDigit()
{
	local DHCPsetting Q R IP
	local Digit Next

	DHCPsetting="$1"

	Q="SELECT IPaddress
FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL AND MACaddress IS NOT NULL AND IPaddress IS NOT NULL
ORDER BY IPaddress"
	R=$(RunSQL "$Q")

	Next=$(echo $DHCPsetting | cut -d, -f1 | cut -d- -f1 | cut -d. -f4)
	if [ -z "$R"  ]; then
		echo "$Next"
		return
	fi

	for IP in $R; do
		Digit=$(echo "$IP" | cut -d. -f4)
		InRange "$Digit" "$DHCPsetting" || continue
		if [ -a "$Next" -lt "$Digit" ] && InRange "$Next" "$DHCPsetting"; then
			echo "$Next"
			return
		fi
		Next=$((Digit+1))
	done
}

# TODO: Core configuration in database; currently using eth0:0 as the internal card
CORE_INTERNAL_INTERFACE="eth0:0"

echo "Setting up Core internal network if necessary: "
Q="SELECT IPaddress
FROM Device
WHERE FK_DeviceTemplate=7"
CORE_INTERNAL_ADDRESS=$(RunSQL "$Q")
if [ -z "$CORE_INTERNAL_ADDRESS" ]; then
	echo "** ERROR ** Empty internal IP address. Will not setup any diskless MD devices until this is fixed **"
	exit 1
fi

/usr/pluto/bin/Network_Setup.sh "$CORE_INTERNAL_INTERFACE" "$CORE_INTERNAL_ADDRESS"

# Create Server-side files

ServerConf=$(ifconfig "$CORE_INTERNAL_INTERFACE" | awk 'NR==2' | perl -ne 's/^.*inet addr:(\S+)\s+Bcast:(\S+)\s+Mask:(\S+).*$/\1,\2,\3/g; print')
CORE_INTERNAL_ADDRESS=$(Field 1 "$ServerConf")
INTERNAL_SUBNET_MASK=$(Field 3 "$ServerConf")
for i in 1 2 3 4; do
	IPDigit=$(echo $CORE_INTERNAL_ADDRESS | cut -d. -f$i)
	MaskDigit=$(echo $INTERNAL_SUBNET_MASK | cut -d. -f$i)
	NetDigit=$(($IPDigit & $MaskDigit))
	INTERNAL_SUBNET="$INTERNAL_SUBNET$Dot$NetDigit" && Dot="."
done

echo "Getting DHCP setting for Core"
Q="SELECT IK_DeviceData
FROM Device_DeviceData
JOIN Device ON PK_Device=FK_Device
JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
WHERE FK_DeviceTemplate=7 AND FK_DeviceData=28"
DHCPsetting=$(RunSQL "$Q")
echo "DHCP setting: $DHCPsetting"

# TODO: filter by installation too (currently only one installation is present, so it works :P)
echo "Getting list of Media Directors"
Q="SELECT IPaddress, MACaddress
FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL AND MACaddress IS NOT NULL"
R=$(RunSQL "$Q")

if [ -n "$DHCPsetting" ]; then
	echo "Allocating IP addresses to IPless Media Directors"
	Prefix=$(echo "$DHCPsetting" | cut -d. -f-3)
	
	for Client in $R; do
		IP=$(Field 1 "$Client")
		MAC=$(Field 2 "$Client")
		Digit=$(echo "$IP" | cut -d. -f4)
		if [ "$IP" == "NULL" -o -z "$IP" ] || ! InRange "$Digit" "$DHCPsetting"; then
			FreeDigit=$(GetFreeDigit "$DHCPsetting")
			if [ -z "$FreeDigit" ]; then
				echo "*** WARNING *** No free IP for moon '$MAC'. Skipping for now"
				continue
			fi
			IP="$Prefix.$FreeDigit"
			echo "Giving IP '$IP' to moon '$MAC'"
			Q="UPDATE Device SET IPaddress='$IP' WHERE MACaddress='$MAC'"
			RunSQL "$Q" >/dev/null
		fi
	done
fi

Q="SELECT IPaddress, MACaddress
FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL AND MACaddress IS NOT NULL AND IPaddress IS NOT NULL"
R=$(RunSQL "$Q")

echo "Setting diskless MD filesystems"
for Client in $R; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")
	
	Q="SELECT *
	FROM Device_DeviceData
	JOIN Device ON PK_Device=FK_Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate=28 AND FK_DeviceData=9 AND IPaddress='$IP' AND MACaddress='$MAC' AND IK_DeviceData='1'"
	Diskless=$(RunSQL "$Q")

	if [ -z "$Diskless" ]; then
		echo "Skipping non-diskless MD '$IP,$MAC'"
		continue
	fi

	if ! [ -d /usr/pluto/diskless/$IP -a -e /usr/pluto/diskless/$IP/etc/diskless.conf ]; then
		echo "Creating initial filesystem for moon '$IP,$MAC'"
		/usr/pluto/bin/Create_DisklessMD_FS.sh "$IP" "$MAC"
	fi
	DisklessR="$DisklessR $Client"
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

# TODO: use dynamic IP range too :)
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
for Client in $DisklessR; do
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
iptables -t nat -F POSTROUTING
for Client in $DisklessR; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")
	echo -n "Setting moon '$IP,$MAC':"
	
	echo -n " fstab"
	cp /usr/pluto/templates/fstab.tmpl /usr/pluto/diskless/$IP/etc/fstab.$$
	ReplaceVars /usr/pluto/diskless/$IP/etc/fstab.$$
	mv /usr/pluto/diskless/$IP/etc/fstab.$$ /usr/pluto/diskless/$IP/etc/fstab
	
	echo -n " hosts"
	cp /usr/pluto/templates/hosts.tmpl /usr/pluto/diskless/$IP/etc/hosts.$$
	ReplaceVars /usr/pluto/diskless/$IP/etc/hosts.$$
	mv /usr/pluto/diskless/$IP/etc/hosts.$$ /usr/pluto/diskless/$IP/etc/hosts

	echo -n " hostname"
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
	mkdir -p /usr/pluto/diskless/$IP/usr/pluto/bin
	/usr/pluto/bin/Start_LocalDevices.sh script "$IP" -d "$PK_Device" >/usr/pluto/diskless/$IP/usr/pluto/bin/Start_LocalDevices_Static.sh
	sed -i 's/localhost/dce_router/g' /usr/pluto/diskless/$IP/usr/pluto/bin/Start_LocalDevices_Static.sh
	chmod +x /usr/pluto/diskless/$IP/usr/pluto/bin/Start_LocalDevices_Static.sh

	echo -n " NAT"
	iptables -t nat -A POSTROUTING -s $IP -j MASQUERADE

	# TODO: MAC-based firewall FORWARD access

	echo
	MoonNumber=$((MoonNumber+1))
done
