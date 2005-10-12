#!/bin/bash

. /usr/pluto/install/Common.sh
. /etc/diskless.conf

ACTIV="http://activate.plutohome.com"
activation_url="$ACTIV/pluto-admin/activation.php"

DIR="/usr/pluto/install"
WhereCode="in diskless.conf"

echo "deb file:/usr/pluto/deb-cache/ sarge main" | cat - /etc/apt/sources.list >/etc/apt/sources.list.2
mv /etc/apt/sources.list.2 /etc/apt/sources.list
apt-get update &>/dev/null

# Format: OK-Device-Code
ok=0
mac=$(/sbin/ifconfig eth0|grep HWaddr|awk '{ print $5; }'|sed s/://g)
while [ "$ok" -eq 0 ]; do
	echo "Getting pre-activation data (if any)"
	answer=$(wget -O - "$activation_url?mac=$mac" 2>/dev/null)
	RetCode=$?
	if [ "$RetCode" -ne 0 ]; then
		try_again "Failed to contact activation server over the Internet" && continue
		echo "$ICS_MSG"
		break
	fi
	result=$(echo $answer |cut -d- -f1)
	if [ "$result" == "OK" ]; then
		Device=$(echo $answer |cut -d- -f2)
		Code=$(echo $answer |cut -d- -f3)
	fi
	ok=1
done

ok=0
while [ "$ok" -eq 0 ]; do
# ask user for activation key
	ok_key=0
	if [ -n "$Device" -a -n "$Code" ]; then
		clear
		echo "Found Device and Activation Code in $WhereCode"
		echo "Device: $Device"
		echo "Activation Code: $Code"
		ok_key=1
		device="$Device"
		code="$Code"
		unset Device Code
	fi

	ok_device="$ok_key"
	while [ "$ok_device" -ne 1 ]; do
		Message1="What device is this going to be?"
		Message2="The device number is listed on step 8 of the new installation wizard at plutohome.com"

		ClearBlue
		echo "$Message1" | fmt
		echo "$Message2" | fmt
		echo -n "Device number: "
		read device
		[ -z "$device" ] && echo "Empty device number" || ok_device=1
		ClearBlack
		
#		device=$(InputBox "$Message1" "$Message2")
#		[ -z "$device" ] && MessageBox "Empty device number" || ok_device=1
	done

	ok_code="$ok_key"
	while [ "$ok_code" -ne 1 ]; do
		Message1="What is your activation code for this installation from plutohome.com?"

		ClearBlue
		echo "$Message1" | fmt
		echo -n "Activation code: "
		read code
		[ -z "$code" ] && echo "Empty activation code" || ok_code=1
		ClearBlack

#		code=$(InputBox "$Message1")
#		[ -z "$code" ] && MessageBox "Empty activation code" || ok_device=1
	done

	activation_key="$device-$code"

# try to get data off the Internet
	ok_internet=0
	while [ "$ok_internet" -eq 0 ]; do
		echo "Getting activation script"
		answer=$(wget -O - "$activation_url?code=$activation_key" 2>/dev/null)
		if [ "$?" -ne 0 ]; then
			try_again "Failed to contact activation server over the Internet" && continue
			echo "$ICS_MSG" | fmt
#			MessageBox "$ICS_MSG"
			exit
		fi
		ok_internet=1
	done

	[ "$ok_internet" -eq 0 ] && break

# verify activation answer
	result=$(echo "$answer" | head -1)
	if [ "$result" != "OK" ]; then
		Message1="Failed getting activation data. Server answer was:"
		Message2="$result"
		try_again "$Message1" "$Message2" && continue
		echo "$NOCODE_MSG" | fmt
#		MessageBox "$NOCODE_MSG"
		exit
	fi

# everything went ok
	echo "Code accepted"

	ok_script=0
	while [ "$ok_script" -eq 0 ]; do
		echo "$answer" | awk 'NR>1' >"$DIR/activation.sh"
		if [ "$(tail -1 "$DIR/activation.sh")" != "#EOF" ]; then
			try_again "Activation script transfer failed" && continue
			echo "$NOACTIV_MSG" | fmt
#			MessageBox "$NOACTIV_MSG"
			exit
		else
			ok_script=1
		fi
	done
	
	Device=$(echo "$activation_key" | cut -d- -f1)
	Code=$(echo "$activation_key" | cut -d- -f2)

	[ "$Type" == "diskless" ] && Host="dcerouter"
	[ "$Type" == "router" ] && Host="localhost"

	pluto_conf="# Pluto config file
MySqlHost = $Host
MySqlUser = root
MySqlPassword = 
MySqlDBName = pluto_main
DCERouter = $Host
MySqlPort = 3306
DCERouterPort = 3450
PK_Device = $Device
Activation_Code = $Code"
	[ -f /etc/pluto.conf ] || echo "$pluto_conf" >/etc/pluto.conf
	chmod 666 /etc/pluto.conf # that 666 is octal, and equals 438 in decimal :)
	
	ok=1
done

[ "$ok" -eq 0 ] && exit 1

clear

chmod +x "$DIR"/activation.sh
if bash -x "$DIR"/activation.sh; then
	echo "Activation went ok"
else
	echo "$ACTIV_MSG" | fmt
fi

/usr/pluto/bin/Initial_Config_Finish.sh
# At this point these scripts should be available (installation finished, didn't it?)
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

apt-get clean

# Replace Initial_Config.sh entry with regular one in inittab
awk '
	/1:23:once/ { next }
	/^#1:2345:respawn/ { print substr($0, 2); next }
	{ print }
' /etc/inittab >/etc/inittab.new
mv -f /etc/inittab.new /etc/inittab
reboot
#init q
exit 0
