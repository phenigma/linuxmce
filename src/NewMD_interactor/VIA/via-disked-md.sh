#!/bin/bash

# stop Pluto software
/usr/pluto/bin/MessageSend localhost 0 -1000 7 0
sleep 5

# remove core-specific packages
Pkgs=(
	pluto-mythtv-plugin pluto-sqlcvs pluto-system-database pluto-telecom-database pluto-std-plugins
	pluto-updateentarea pluto-website-admin pluto-website-support pluto-database-settings
	pluto-local-database pluto-media-database pluto-security-database mysql-server mysql-server-4.1
	pluto-asterisk pluto-voicemail-monitor pluto-remoteassistance
)

for Package in "${Pkgs[@]}"; do
	apt-get -y --purge remove "$Package"
done

# backup original config
if [[ ! -f /etc/network/interfaces.pbackup ]]; then
	cp /etc/network/interfaces{,.pbackup}
fi

# setup network
/etc/init.d/dhcp3-server stop
/etc/init.d/samba stop
ifdown eth0:0
ifconfig eth0:0 down

. /usr/pluto/bin/Config_Ops.sh

ConfSet DisableFirewall 1
/usr/pluto/bin/Network_Firewall.sh

echo "#loopback
auto lo
iface lo inet loopback

#network card
auto eth0
iface eth0 inet dhcp
" >/etc/network/interfaces

# get provisional IP
/etc/init.d/networking restart

# get network data
MyIP=$(/sbin/ifconfig eth0 | awk 'NR==2 { print substr($2, index($2, ":") + 1) }')
MyMAC=$(/sbin/ifconfig eth0 | awk 'NR==1 { print $5 }')
Gateway=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $2 }')

Architecture="686"

echo "IP: $MyIP; MAC: $MyMAC; Gateway: $Gateway; Architecture: $Architecture"

if [[ -z "$MyIP" || -z "$MyMAC" ]]; then
	echo "Failed to get address from DHCP"
	exit 1
fi

# kill existing interactor server
InteractorPID=$(pgrep -f '^/bin/bash.*Start_NewMD_interactor.sh$')
if [[ -n "$InteractorPID" ]]; then
	kill "$InteractorPID"
fi

# run interactor client
if ! /usr/pluto/install/via-interactor "$Gateway" "$MyIP" "$MyMAC" "$Architecture"; then
	echo "Interactor client exited with error"
	exit 1
fi

if [[ ! -f /etc/Disked_DeviceID ]]; then
	echo "Interactor didn't write our device ID"
	exit 1
fi

# get allocated IP
/etc/init.d/networking restart

# make mtab a symlink to /proc/mounts
rm -f /etc/mtab
ln -s /proc/mounts /etc/mtab

# set up fstab
fstab_header="#Pluto fstab entries"
if ! grep -qF "$fstab_header" /etc/fstab; then
	sed -i '/home/ d' /etc/fstab
	echo "$fstab_header
$Gateway:/usr/pluto/var /usr/pluto/var nfs intr,udp,rsize=32768,wsize=32768,retrans=10,timeo=50 1 1
$Gateway:/usr/pluto/orbiter /usr/pluto/orbiter nfs intr,udp,rsize=32768,wsize=32768,retrans=10,timeo=50 1 1
$Gateway:/usr/pluto/keys /usr/pluto/keys nfs intr,udp,rsize=32768,wsize=32768,retrans=10,timeo=50 1 1
//$Gateway/home /home cifs credentials=/usr/pluto/var/sambaCredentials.secret 1 1" >>/etc/fstab
fi
umount -l /home
mount -a

# get configs from server
DeviceID=$(</etc/Disked_DeviceID)
tar -C / --exclude='usr/pluto/install' --exclude='etc/apt/sources.list' -xzf /usr/pluto/var/Disked_$DeviceID.tar.gz

ConfEval

ConfSet AutostartCore 1
ConfSet AutostartMedia 1
ConfSet PK_Distro 1

# reconfigure network with static settings
/etc/init.d/networking restart
/etc/init.d/nis restart

rm -f /etc/hosts

# logging place
rm -f /var/log/pluto
mkdir -p /var/log/pluto

. /usr/pluto/bin/SQL_Ops.sh

# setup database
Q="
	SELECT PK_Device
	FROM Device
	WHERE FK_DeviceTemplate=62 AND FK_Device_ControlledVia=$DeviceID
"
OrbiterDev="$(RunSQL "$Q")"

Q="
	SELECT PK_Device
	FROM Device
	WHERE FK_DeviceTemplate=5 AND FK_Device_ControlledVia=$OrbiterDev
"
XineDev="$(RunSQL "$Q")"

# Video settings
RunSQL "
	UPDATE Device_DeviceData
	SET IK_DeviceData='1024 768/60'
	WHERE FK_Device=$DeviceID AND FK_DeviceData=89
"

# PK_Size
RunSQL "
	UPDATE Device_DeviceData
	SET IK_DeviceData='6'
	WHERE FK_DeviceData=25 AND FK_Device=$OrbiterDev
"

# Use alpha blended UI
RunSQL "
	UPDATE Device_DeviceData
	SET IK_DeviceData='1'
	WHERE FK_DeviceData=169 AND FK_Device=$OrbiterDev
"

# Use OpenGL effects
RunSQL "
	UPDATE Device_DeviceData
	SET IK_DeviceData='1'
	WHERE FK_DeviceData=172 AND FK_Device=$OrbiterDev
"

# PK_UI
RunSQL "
	UPDATE Device_DeviceData
	SET IK_DeviceData='4'
	WHERE FK_DeviceData=104 AND FK_Device=$OrbiterDev
"

# Hardware acceleration
RunSQL "
	UPDATE Device_DeviceData
	SET IK_DeviceData='cle266x11'
	WHERE FK_DeviceData=168 AND FK_Device=$XineDev
"

# remove DCE router report on first console
sed -ir '/^1:2345:/ s,^.*$,1:2345:respawn:/sbin/getty 38400 tty1,g' /etc/inittab
#init q

# changes passwd,group and shadow files to look over nis too
if ! grep -q "+::::::"   /etc/passwd ;then
	cat /etc/passwd  > /etc/passwd.$$
	echo "+::::::"  >> /etc/passwd.$$
	mv /etc/passwd.$$ /etc/passwd
fi
if ! grep -q "+::::::::" /etc/shadow ;then
	cat /etc/shadow  > /etc/shadow.$$
	echo "+::::::::" >> /etc/shadow.$$
	mv /etc/shadow.$$ /etc/shadow
fi
if ! grep -q "+:::" /etc/group ;then
	cat /etc/group | grep -v "^public:" > /etc/group.$$
	echo "+:::"  >> /etc/group.$$
	mv /etc/group.$$ /etc/group
fi

# call for a regen
/usr/pluto/bin/MessageSend $DCERouter -targetType template "$OrbiterDev" 12 1 266 2 "$OrbiterDev" 21 "-r" 24 "Y"

# generate startup scripts (core ran /usr/pluto/bin/Update_StartupScrips.sh for us)
/usr/pluto/bin/generateRcScripts.sh

sync
echo "Script finished. Press Control+Alt+Del to reboot."
exit 0
