#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh

DlDir="/usr/pluto/diskless"

# vars:
# CORE_INTERNAL_ADDRESS
# INTERNAL_SUBNET
# INTERNAL_SUBNET_MASK
# MOON_ENTRIES
# MOON_ADDRESS
# MOON_IP
# DYNAMIC_IP_RANGE
# KERNEL_VERSION
# MOON_HOSTS

KERNEL_VERSION="$(uname -r)"

ReplaceVars()
{
	# TODO
	local File Commands Vars VarValue SedCmd
	File="$1"
	
	Vars="CORE_INTERNAL_ADDRESS INTERNAL_SUBNET INTERNAL_SUBNET_MASK MOON_ENTRIES MOON_ADDRESS DYNAMIC_IP_RANGE KERNEL_VERSION MOON_HOSTS MOON_IP"

	for i in $Vars; do
		eval "VarValue=\"\$$i\""
		VarValue=${VarValue//\//\\\/}
		VarValue=$(echo "$VarValue" | sed 's/^ *//g; s/ *$//g')
		SedCmd="s/%$i%/$VarValue/g"
		[ -z "$Commands" ] && Commands="$SedCmd" || Commands="$Commands; s/%$i%/$VarValue/g"
	done
	sed -i "$Commands" $File
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
		if [ "$Next" -lt "$Digit" ] && InRange "$Next" "$DHCPsetting"; then
			echo "$Next"
			return
		fi
		Next=$((Digit+1))
	done
	if InRange "$Next" "$DHCPsetting"; then
		echo "$Next"
	fi
}

CORE_INTERNAL_INTERFACE="$IntIf"

# Create Server-side files

CORE_INTERNAL_ADDRESS="$IntIP"
INTERNAL_SUBNET_MASK="$IntNetmask"
for i in 1 2 3 4; do
	IPDigit=$(echo $CORE_INTERNAL_ADDRESS | cut -d. -f$i)
	MaskDigit=$(echo $INTERNAL_SUBNET_MASK | cut -d. -f$i)
	NetDigit=$(($IPDigit & $MaskDigit))
	INTERNAL_SUBNET="$INTERNAL_SUBNET$Dot$NetDigit" && Dot="."
done

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
else
	echo "Diskless MDs can't exist in the current setup (no DHCP). Not setting up any."
	exit
fi

Q="SELECT IPaddress, MACaddress, PK_Device
FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL AND MACaddress IS NOT NULL AND IPaddress IS NOT NULL"
R=$(RunSQL "$Q")

echo "Setting diskless MD filesystems"
for Client in $R; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")
	Device=$(Field 3 "$Client")
	DlPath="$DlDir/$IP"
	
	Q="SELECT PK_Device
	FROM Device_DeviceData
	JOIN Device ON PK_Device=FK_Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate=28 AND FK_DeviceData=9 AND IPaddress='$IP' AND MACaddress='$MAC' AND IK_DeviceData='1'"
	Diskless=$(RunSQL "$Q")

	if [ -z "$Diskless" ]; then
		echo "Skipping non-diskless MD '$IP , $MAC' ($Device)"
		continue
	fi

	/usr/pluto/bin/Create_DisklessMD_FS.sh "$IP" "$MAC" "$Device" "$Activation_Code"
	DisklessR="$DisklessR $Client"
done

echo "Setting up /etc/exports"
cp /usr/pluto/templates/exports.tmpl /etc/exports.$$
for Client in $R; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")
	DlPath="$DlDir/$IP"
	echo "$DlPath $IP/255.255.255.255(rw,no_root_squash,no_all_squash,sync)" >>/etc/exports.$$
done
ReplaceVars /etc/exports.$$
mv /etc/exports.$$ /etc/exports
#/etc/init.d/nfs-kernel-server restart
if ! /sbin/showmount -e localhost &>/dev/null; then
	/etc/init.d/nfs-common start
	/etc/init.d/nfs-kernel-server start
fi
/usr/sbin/exportfs -ra

echo "Setting up /etc/dhcp3/dhcpd.conf, /etc/hosts"
cp /usr/pluto/templates/dhcpd.conf.tmpl /etc/dhcp3/dhcpd.conf.$$
MoonNumber=1
MOON_ENTRIES=""
MOON_HOSTS=""
for Client in $R; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")

	if ! /usr/pluto/bin/CheckMAC.sh "$MAC"; then
		echo "Invalid MAC address for moon '$IP , $MAC'"
		continue;
	fi
	# verbatim escape sequences used by red in ReplaceVariables; don't replace them with the real thing!
	MOON_ENTRIES="$MOON_ENTRIES\n\thost moon$MoonNumber { hardware ethernet $MAC; fixed-address $IP; }"
	MOON_HOSTS="$MOON_HOSTS\n$IP\tmoon$MoonNumber"
	MoonNumber=$((MoonNumber+1))
done

DYNAMIC_IP_RANGE=
if [ "${DHCPsetting%,*}" != "$DHCPsetting" ]; then
	DHCPRange="${DHCPsetting#*,}"
	Range1="${DHCPRange%-*}"
	Range2="${DHCPRange#*-}"

	DYNAMIC_IP_RANGE=$(printf "%s" "\n\tpool {\n\t\tallow unknown-clients;\n\t\trange $Range1 $Range2;\n\t}")
fi

ReplaceVars /etc/dhcp3/dhcpd.conf.$$
ReplaceVars /etc/hosts
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
for Client in $DisklessR; do
	IP=$(Field 1 "$Client")
	MAC=$(Field 2 "$Client")
	MOON_ADDRESS="$IP moon$MoonNumber"
	MOON_IP="$IP"
	echo -n "Setting moon '$IP,$MAC':"
	
	DlPath="$DlDir/$IP"
	
	echo -n " fstab"
	cp /usr/pluto/templates/fstab.tmpl $DlPath/etc/fstab.$$
	ReplaceVars $DlPath/etc/fstab.$$
	mv $DlPath/etc/fstab.$$ $DlPath/etc/fstab
	
	echo -n " hosts"
	cp /usr/pluto/templates/hosts.tmpl $DlPath/etc/hosts.$$
	ReplaceVars $DlPath/etc/hosts.$$
	mv $DlPath/etc/hosts.$$ $DlPath/etc/hosts

	echo -n " hostname"
	echo "moon$MoonNumber" >$DlPath/etc/hostname

#	echo -n " Startup_Scripts"
#	/etc/init.d/Startup_Scripts.sh script "$IP" >$DlPath/etc/pluto.startup

	echo -n " MythTV_DB_Settings"
	mkdir -p $DlPath/etc/mythtv
	cp /etc/mythtv/mysql.txt $DlPath/etc/mythtv/mysql.txt.$$
	SedCmd="s/^DBHostName.*/DBHostName=dcerouter/g"
	sed -i "$SedCmd" $DlPath/etc/mythtv/mysql.txt.$$
	mv $DlPath/etc/mythtv/mysql.txt.$$ $DlPath/etc/mythtv/mysql.txt

#	echo -n " Start_LocalDevices"
#	Q="SELECT PK_Device FROM Device WHERE IPaddress='$IP' AND MACaddress='$MAC' LIMIT 1"
#	PK_Device=$(RunSQL "$Q")
#	mkdir -p $DlPath/usr/pluto/bin
#	/usr/pluto/bin/Start_LocalDevices.sh script "$IP" -d "$PK_Device" >$DlPath/usr/pluto/bin/Start_LocalDevices_Static.sh
#	sed -i 's/localhost/dcerouter/g' $DlPath/usr/pluto/bin/Start_LocalDevices_Static.sh
#	chmod +x $DlPath/usr/pluto/bin/Start_LocalDevices_Static.sh

	echo -n " MySQL_access"
	Q="GRANT ALL PRIVILEGES ON pluto_main.* TO 'root'@'$IP';
	GRANT ALL PRIVILEGES ON mythconverg.* TO 'root'@'$IP';"
	echo "$Q" | /usr/bin/mysql

	echo -n " Install"
	mkdir -p $DlPath/usr/pluto/install
	for Script in ConfirmDependencies_Debian.sh Common.sh Download_CVS.sh Download_Direct.sh Download_SVN.sh Initial_Config_Real.sh; do
		cp /usr/pluto/install/$Script $DlPath/usr/pluto/install
	done
	sed '/^Type=/ s/^.*$/Type="diskless"/' /usr/pluto/install/Initial_Config.sh >$DlPath/usr/pluto/install/Initial_Config.sh
	chmod +x $DlPath/usr/pluto/install/Initial_Config.sh
	mkdir -p $DlPath/usr/pluto/deb-cache

	echo -n " Timezone"
	if [ -f /etc/timezone ]; then
		TimeZone="$(cat /etc/timezone)"
	else
		TimeZone="$(readlink /etc/localtime)"
		TimeZone="${TimeZone#/usr/share/zoneinfo/}"
	fi
	echo "$TimeZone" >/etc/timezone
	[ -f "$DlPath/etc/localtime" ] && rm -f $DlPath/etc/localtime # in case it's a file from the old (wrong) way of setting this up
	ln -sf /usr/share/zoneinfo/$TimeZone $DlPath/etc/localtime

	echo -n " syslog"
	SysLogCfg1="*.*;auth,authpriv.none		/dev/tty12"
	SysLogCfg2="*.*;auth,authpriv.none		@dcerouter"
	if ! grep -qF "$SysLogCfg1" $DlPath/etc/syslog.conf; then
		echo "$SysLogCfg1" >>$DlPath/etc/syslog.conf
	fi
	if ! grep -qF "$SysLogCfg2" $DlPath/etc/syslog.conf; then
		echo "$SysLogCfg2" >>$DlPath/etc/syslog.conf
	fi

	echo
	MoonNumber=$((MoonNumber+1))
done

Q="FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql
