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
Vars="CORE_INTERNAL_ADDRESS INTERNAL_SUBNET INTERNAL_SUBNET_MASK MOON_ENTRIES MOON_ADDRESS DYNAMIC_IP_RANGE KERNEL_VERSION MOON_HOSTS MOON_IP"

GetFreeDigit()
{
	local DHCPsetting Q R IP
	local Digit Next

	DHCPsetting="$1"

	Q="SELECT IPaddress
		FROM Device
		WHERE MACaddress IS NOT NULL AND IPaddress IS NOT NULL AND MACaddress!='' AND IPaddress!=''"
	R=$(RunSQL "$Q" | sort -t. -n -k1,1 -k2,2 -k3,3 -k4,4)

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

if [ -z "$DHCPsetting" ]; then
	echo "Diskless MDs can't exist in the current setup (no DHCP). Not setting up any."
	exit
fi

# TODO: filter by installation too (currently only one installation is present, so it works :P)
echo "Getting list of Media Directors"
Q="SELECT PK_Device, IPaddress, MACaddress, Device.Description
FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL"
R=$(RunSQL "$Q")

Prefix=$(echo "$DHCPsetting" | cut -d. -f-3)
MoonNumber=1
MOON_ENTRIES=""
MOON_HOSTS=""

cp /usr/pluto/templates/exports.tmpl /etc/exports.$$
cp /usr/pluto/templates/dhcpd.conf.tmpl /etc/dhcp3/dhcpd.conf.$$
mkdir -p /tftpboot/pxelinux.cfg

for Client in $R; do
	PK_Device=$(Field 1 "$Client")
	IP=$(Field 2 "$Client")
	MAC=$(Field 3 "$Client")
	Description=$(Field 4 "$Client")
	
	echo "Processing moon $MoonNumber ($Description; $MAC; $PK_Device)"
	Digit=$(echo "$IP" | cut -d. -sf4)

	# TODO: replace InRage with InSubnet thing
	if [ "$IP" == "NULL" -o -z "$IP" ] || ! InRange "$Digit" "$DHCPsetting"; then
		FreeDigit=$(GetFreeDigit "$DHCPsetting")
		echo "* IP allocation"
		if [ -z "$FreeDigit" ]; then
			echo "*** WARNING *** No free IP"
			continue
		fi
		IP="$Prefix.$FreeDigit"
		echo "* Allocating IP '$IP'"
		Q="UPDATE Device SET IPaddress='$IP' WHERE PK_Device='$PK_Device'"
		RunSQL "$Q" >/dev/null
	fi

	DlPath="$DlDir/$IP"
	HexIP=$(gethostip -x "$IP")
	lcdMAC=$(echo ${MAC//:/-} | tr 'A-Z' 'a-z')
	MAC=$(echo ${MAC//-/:} | tr 'a-z' 'A-Z')

	MOON_IP="$IP"
	MOON_ADDRESS="$IP moon$MoonNumber"
	/usr/pluto/bin/CheckMAC.sh "$MAC"
	ValidMAC=$?
	
	Q="SELECT PK_Device
	FROM Device_DeviceData
	JOIN Device ON PK_Device=FK_Device
	JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
	WHERE FK_DeviceTemplate=28 AND FK_DeviceData=9 AND IK_DeviceData='1' AND PK_Device='$PK_Device'"
	Diskless=$(RunSQL "$Q")

	if [ -n "$Diskless" ]; then
		echo "* Diskless filesystem"
		/usr/pluto/bin/Create_DisklessMD_FS.sh "$IP" "$MAC" "$PK_Device" "$Activation_Code"
		
		if [ "$ValidMAC" -eq 0 ]; then
			echo "* Setting up /tftpboot/pxelinux.cfg/01-$lcdMAC"
			cp /usr/pluto/templates/pxelinux.tmpl /tftpboot/pxelinux.cfg/01-$lcdMAC.$$
			ReplaceVars /tftpboot/pxelinux.cfg/01-$lcdMAC.$$
			mv /tftpboot/pxelinux.cfg/01-$lcdMAC.$$ /tftpboot/pxelinux.cfg/01-$lcdMAC
		else
			echo "* MAC address is wrong. You won't be able to boot this until you correct it."
		fi

		echo "* Setting local files"
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

		echo -n " MythTV_DB_Settings"
		mkdir -p $DlPath/etc/mythtv
		cp /etc/mythtv/mysql.txt $DlPath/etc/mythtv/mysql.txt.$$
		SedCmd="s/^DBHostName.*/DBHostName=dcerouter/g"
		sed -i "$SedCmd" $DlPath/etc/mythtv/mysql.txt.$$
		mv $DlPath/etc/mythtv/mysql.txt.$$ $DlPath/etc/mythtv/mysql.txt

		echo -n " MySQL_access"
		Q="GRANT ALL PRIVILEGES ON *.* TO 'root'@$IP;
		GRANT ALL PRIVILEGES ON *.* TO 'eib'@$IP"
		echo "$Q" | /usr/bin/mysql

		echo -n " Install"
		mkdir -p $DlPath/usr/pluto/install
		for Stuff in ConfirmDependencies_Debian.sh Common.sh Download_CVS.sh Download_Direct.sh Download_SVN.sh Initial_Config_Real.sh ramdisk.tar.bz2; do
			cp /usr/pluto/install/$Stuff $DlPath/usr/pluto/install
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

		echo -n " apt"
		sed 's/localhost/dcerouter/g' /etc/apt/apt.conf.d/30pluto > $DlPath/etc/apt/apt.conf.d/30pluto

		echo " debconf"
		awk '/Name: debconf\/frontend/,/^$/ {if ($1 == "Value:") print "Value: Noninteractive"; else print; next}
			{print}' $DlPath/var/cache/debconf/config.dat > $DlPath/var/cache/debconf/config.dat.$$
		mv $DlPath/var/cache/debconf/config.dat.$$ $DlPath/var/cache/debconf/config.dat

		echo
		
		echo "* Adding root to exports"
		echo "$DlPath $IP/255.255.255.255(rw,no_root_squash,no_all_squash,sync)" >>/etc/exports.$$
	fi

	echo "* Adding to dhcp"
	if [ "$ValidMAC" -eq 0 ]; then
		# verbatim escape sequences used by red in ReplaceVariables; don't replace them with the real thing!
		MOON_ENTRIES="$MOON_ENTRIES\n\thost moon$MoonNumber { hardware ethernet $MAC; fixed-address $IP; }"
	else
		echo "* Invalid MAC address: '$MAC'"
	fi

	echo "* Adding to hosts"
	MOON_HOSTS="$MOON_HOSTS\n$IP\tmoon$MoonNumber"

	: $((MoonNumber+=1))
done

DYNAMIC_IP_RANGE=
if [ "${DHCPsetting%,*}" != "$DHCPsetting" ]; then
	DHCPRange="${DHCPsetting#*,}"
	Range1="${DHCPRange%-*}"
	Range2="${DHCPRange#*-}"

	DYNAMIC_IP_RANGE=$(printf "%s" "\n\tpool {\n\t\tallow unknown-clients;\n\t\trange $Range1 $Range2;\n\t}")
fi

ReplaceVars /etc/exports.$$
ReplaceVars /etc/dhcp3/dhcpd.conf.$$
ReplaceVars /etc/hosts

mv /etc/exports.$$ /etc/exports
mv /etc/dhcp3/dhcpd.conf.$$ /etc/dhcp3/dhcpd.conf

if ! /sbin/showmount -e localhost &>/dev/null; then
	/etc/init.d/nfs-common start
	/etc/init.d/nfs-kernel-server start
fi
/usr/sbin/exportfs -ra
/etc/init.d/dhcp3-server restart

Q="FLUSH PRIVILEGES"
RunSQL "$Q"

echo "Finished setting up network boot for media directors."
echo "If new media director(s) were added, do a quick reload router."

