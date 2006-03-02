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
if [[ $hasRecords -ne 0 ]]; then
	echo "Database already setup";
	SkipDatabase="yes"
fi

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/Update_StartupScrips.sh
/usr/pluto/bin/generateRcScripts.sh

mkdir -p /tftpboot/pxelinux.cfg
cp /usr/lib/syslinux/pxelinux.0 /tftpboot

# Changed from 2.0.0.10 to 2.0.0.11: diskless filesystems were moved to /home
if [[ ! -L /usr/pluto/diskless && -d /usr/pluto/diskless ]]; then
	mv /usr/pluto/diskless /home
else
	mkdir -p /home/diskless
fi
rm -f /usr/pluto/diskless
ln -s /home/diskless /usr/pluto/diskless

# Changed from 2.0.0.24 to 2.0.0.25: pluto logs and core dumps were moved to /home
if [[ ! -L /usr/pluto/coredump && -d /usr/pluto/coredump ]]; then
	mv /usr/pluto/coredump /home
else
	mkdir -p /home/coredump
fi
rm -f /usr/pluto/coredump
ln -s /home/coredump /usr/pluto/coredump

if [[ ! -L /var/log/pluto && -d /var/log/pluto ]]; then
	mkdir -p /home/logs
	mv /var/log/pluto /home/logs
else
	mkdir -p /home/logs/pluto
fi
rm -f /var/log/pluto
ln -s /home/logs/pluto /var/log/pluto

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
if [[ -f /usr/pluto/bin/Config_Ops.sh ]]; then
	. /usr/pluto/bin/Config_Ops.sh
fi

export PS1='\h_'\$PK_Installation':\w\$ '"

if ! grep -q nullglob /root/.profile; then
	echo "shopt -u nullglob # Pluto disable nullglob for shell" >>/root/.profile
fi

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

modules="ip_conntrack ip_conntrack_ftp ip_conntrack_irc ip_nat_ftp ip_nat_irc"

for module in $modules; do
	if ! grep -q "$module" /etc/modules; then
		echo "$module" >>/etc/modules
	fi
	modprobe $module
done
