#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# 2005-04-11 * Disabled. Considered not needed anymore
#echo "Converting all tables except 'mysql' to InnoDB (this may take a while)"
#DBs=$(RunSQL 'SHOW DATABASES')
#for DB in $DBs; do
#	[[ "$DB" == mysql ]] && continue
#	Tables=$(RunSQL "SHOW TABLES FROM $DB")
#	for Table in $Tables; do
#		RunSQL "ALTER TABLE \`$DB\`.\`$Table\` TYPE=$DefTableType"
#	done
#done
#echo "Finished converting tables to InnoDB"

# When we're getting a new software version, be sure we do a full regen 
# on all the orbiters

device="$PK_Device"
code="$Activation_Code"

echo "setting up dce router2"
hasRecords=$(RunSQL "SELECT count(PK_Installation) FROM Installation")
if [ $hasRecords -ne 0 ]; then
	echo "Database already setup";
	SkipDatabase="yes"
fi

# Temporary code to remove everything related to Mozilla_Plugin
RunSQL "DELETE FROM Device where FK_DeviceTemplate=30" # We got rid of Mozilla Plugin
rm -f /usr/pluto/bin/Mozilla_Plugin.so
#apt-get -y remove pluto-mozilla-plugin pluto-src-mozilla-plugin || true

# Temporary code since we changed the way these scenarios are created
RunSQL "DELETE FROM CommandGroup where FK_Template in (1,2,3,4,5,6,7,8,13,14,16,17,18)"

# If any PC's are missing app servers, add them.  There was a bug in .29 that it didn't have app servers
Q="SELECT PC.PK_Device FROM Device AS PC
JOIN DeviceTemplate AS PCDT ON PC.FK_DeviceTemplate=PCDT.PK_DeviceTemplate
LEFT JOIN Device AS APP ON APP.FK_Device_ControlledVia=PC.PK_Device AND APP.FK_DeviceTemplate=26
WHERE PCDT.FK_DeviceCategory IN (6,7,8) AND APP.PK_Device IS NULL"

PCS=$(RunSQL "$Q")

for PCDev in $PCS; do
	/usr/pluto/bin/CreateDevice -d 26 -C $PCDev 
done

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/Update_StartupScrips.sh

mkdir -p /tftpboot/pxelinux.cfg
cp /usr/lib/syslinux/pxelinux.0 /tftpboot

# Changed from 2.0.0.10 to 2.0.0.11: diskless filesystems were moved to /home
if [ ! -L /usr/pluto/diskless -a -d /usr/pluto/diskless ]; then
	mv /usr/pluto/diskless /home
fi
mkdir -p /home/diskless
ln -sf /home/diskless /usr/pluto/diskless

# Changed from 2.0.0.24 to 2.0.0.25: pluto logs and core dumps were moved to /home
if [ ! -L /usr/pluto/coredump -a -d /usr/pluto/coredump ]; then
	mv /usr/pluto/coredump /home
fi
mkdir -p /home/coredump
ln -sf /home/coredump /usr/pluto/coredump

if [ ! -L /var/log/pluto -a -d /var/log/pluto ]; then
	mkdir -p /home/logs
	mv /var/log/pluto /home/logs
fi
mkdir -p /home/logs/pluto
ln -sf /home/logs/pluto /var/log/pluto

# update atftp entry in inet.d
update-inetd --remove tftp
update-inetd --group BOOT --add "tftp        dgram   udp wait    nobody /usr/sbin/tcpd /usr/sbin/in.tftpd --tftpd-timeout 300 --retry-timeout 5     --mcast-port 1758 --mcast-addr 239.255.0.0-255 --maxthread 100 --verbose=5 --no-blksize /tftpboot"

### Configure polipo, frox and apt
# /etc/polipo/config
polipo_conf='# Pluto config for polipo
proxyAddress = "0.0.0.0"
proxyPort = 8123
allowedClients = 0.0.0.0/0
logFile = /var/log/polipo.log
relaxTransparency = maybe
dnsUseGethostbyname = true
'
# /etc/frox.conf
frox_conf='# Pluto config for frox
Port 8124
User frox
Group frox
WorkingDir /var/cache/frox
DontChroot yes
LogLevel 20
LogFile /var/log/frox.log
PidFile /var/cache/frox/frox.pid
BounceDefend yes
CacheModule local
CacheSize 500
CacheAll no
DoNTP yes
MaxForks 50
MaxForksPerHost 4
ACL Allow * - *
'

echo -n "$polipo_conf" >/etc/polipo/config
echo -n "$frox_conf" >/etc/frox.conf
/usr/sbin/adduser --system --group --home /var/cache/frox --disabled-login --disabled-password frox &>/dev/null|| /bin/true
echo "RUN_DAEMON=yes" >/etc/default/frox
/etc/init.d/frox restart
/etc/init.d/polipo restart

bash_flag="# Pluto - bash root prompt"
bash_prompt="$bash_flag
if [ -f /usr/pluto/bin/Config_Ops.sh ]; then
	. /usr/pluto/bin/Config_Ops.sh
fi

export PS1='\h_'\$PK_Installation':\w\$ '"

if ! grep -qF "$bash_flag" /root/.bashrc; then
	awk '/PS1=/ {next} {print}' /root/.bashrc >/root/.bashrc.$$
	echo "$bash_prompt" >>/root/.bashrc.$$
	mv -f /root/.bashrc.$$ /root/.bashrc
fi

echo "Adding user_xattr to filesystems that support it and remounting them"
awk '/^#/ || ($3 != "ext2" && $3 != "ext3" && $3 != "xfs") || /user_xattr/ {print; next}
	{printf "%-15s %-15s %-7s %-15s %-7s %-7s\n", $1, $2, $3, $4 ",user_xattr", $5, $6}' /etc/fstab >/etc/fstab.$$
mv /etc/fstab.$$ /etc/fstab
awk '!/^#/ && ($3 == "ext2" || $3 == "ext3" || $3 == "xfs") {print $2}' /etc/fstab | while read line; do
	echo -n "$line "
	mount -o remount $line && echo "Ok" || echo "Failed"
done

[[ -z "$RA_CheckRemotePort" ]] && ConfSet RA_CheckRemotePort 1
rm -rf /var/cache/polipo/* || /bin/true

# prevent slapd from starting at boot
update-rc.d -f slapd remove || /bin/true
