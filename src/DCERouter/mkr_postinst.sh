#!/bin/bash
#enable MySQL networking
MyCnf=/etc/mysql/my.cnf
DefTableType=innodb
sed -i "s/^skip-networking/#skip-networking/; s/^skip-innodb/#skip-innodb/; s/^default-table-type=.*$/default-table-type=$DefTableType/" "$MyCnf"
grep '^default-table-type=' "$MyCnf" || echo "default-table-type=$DefTableType" >>"$MyCnf"
echo "GRANT ALL PRIVILEGES ON pluto_main.* to 'root'@'127.0.0.1';" | mysql
/etc/init.d/mysql restart

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

echo "Converting all tables except mysql to InnoDB (this may take a while)"
DBs=$(RunSQL 'SHOW DATABASES')
for DB in $DBs; do
	[[ "$DB" == mysql ]] && continue
	Tables=$(RunSQL "SHOW TABLES FROM $DB")
	for Table in $Tables; do
		RunSQL "ALTER TABLE \`$DB\`.\`$Table\` TYPE=$DefTableType"
	done
done
echo "Finished converting tables to InnoDB"

device="$PK_Device"
code="$Activation_Code"

chmod 700 /usr/pluto/keys/id_dsa_remoteassistance
echo "setting up dce router2"
hasRecords=$(RunSQL "SELECT count(PK_Installation) FROM Installation")
if [ $hasRecords -ne 0 ]; then
	echo "Database already setup";
	SkipDatabase="yes"
fi

if [ "$SkipDatabase" != "yes" ]; then
	DIDsql="/usr/pluto/install/database_initial_data.sql"
	wget -O "$DIDsql" "http://plutohome.com/initialdata.php?code=$code&device=$device" 2>/dev/null
	if [ "$?" -ne 0 ]; then
		echo "Failed to get initial data. Wget returned $?"
		exit 1
	else
		Header="$(head -1 "$DIDsql")"
		Tail="$(tail -1 "$DIDsql")"
		if ! echo "$Header" | grep -q "^-- OK."; then
			echo "Failed to get initial data. Server answer was:"
			echo "$Header"
			exit 1
		fi
		if [ "$Tail" != "-- EOF" ]; then
			echo "Failed to get initial data. Incomplete download."
			exit 1
		fi

		echo "Initial data download complete."
	fi

	size=$(stat -c "%s" /usr/pluto/install/database_initial_data.sql)
	if [ "$size" -lt 100 ]; then
		echo "01 Error getting your initial data."
		echo "This installation will not work correctly."
		echo "The installation will continue anyway, but"
		echo "you will need to contact tech support to correct"
		echo "this before you can use it."
		echo "Press any key to continue."
		read
		cat /usr/pluto/install/database_initial_data.sql
	else
		/usr/bin/mysql pluto_main </usr/pluto/install/database_initial_data.sql
	fi

	devices=$(echo "SELECT PK_Device FROM Device;" | /usr/bin/mysql pluto_main | tail +2 | tr '\n' ' ')

	for i in $devices; do
		echo "Running device $i"

		#run the following with the device
		# Add missing data by template
		Q1="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT PK_Device,DeviceTemplate_DeviceData.FK_DeviceData,DeviceTemplate_DeviceData.IK_DeviceData
		FROM Device
		JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceTemplate_DeviceData on DeviceTemplate_DeviceData.FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceTemplate_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=$i;"

		# Add for the category
		Q2="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT PK_Device,DeviceCategory_DeviceData.FK_DeviceData,DeviceCategory_DeviceData.IK_DeviceData
		FROM Device
		JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceTemplate.FK_DeviceCategory
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=$i;"

		# Add for parent's category
		Q3="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT PK_Device,DeviceCategory_DeviceData.FK_DeviceData,DeviceCategory_DeviceData.IK_DeviceData
		FROM Device
		JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceCategory ON DeviceTemplate.FK_DeviceCategory=PK_DeviceCategory
		JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceCategory.FK_DeviceCategory_Parent
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=$i;"

		(echo "$Q1"; echo "$Q2"; echo "$Q3";) | /usr/bin/mysql pluto_main
	done

	# Add to Installation_Users
	Q4="INSERT INTO Installation_Users(FK_Installation,FK_Users,userCanModifyInstallation)
	SELECT PK_Installation,PK_Users,1
	FROM Installation
	JOIN Users;"

	echo "$Q4" | /usr/bin/mysql pluto_main

	# LDAP setup

	SlapdConf="/etc/ldap/slapd.conf"
	#SlapdInc="/etc/ldap/pluto.databases"

	mkdir -p /usr/share/ldap/data

	ConfAppend="
# Pluto

allow bind_v2

database bdb
suffix \"dc=plutohome, dc=org\"
rootdn \"cn=admin, dc=plutohome, dc=org\"
rootpw secret
directory /usr/share/ldap/data
schemacheck on
lastmod on
index cn,sn,st eq,pres,sub


access to dn=\".*dc=([^,]+),dc=plutohome,dc=org\"
       by dn=\"uid=\$1,ou=users,dc=plutohome,dc=org\" write
       by * read

access to *
       by * read

access to *
       by dn=\"cn=admin,dc=plutohome,dc=org\" write
       by dn=\"cn=admin,dc=plutohome,dc=org\" read

"

	grep -F "# Pluto" "$SlapdConf" >/dev/null || echo "$ConfAppend" >> "$SlapdConf"
	
	/usr/pluto/bin/SetupUsers.sh
	/usr/pluto/bin/Update_StartupScrips.sh
fi

mkdir -p /tftpboot/pxelinux.cfg
cp /usr/lib/syslinux/pxelinux.0 /tftpboot

# Changed from 2.0.0.10 to 2.0.0.11: diskless filesystems were moved to /home
if [ ! -L /usr/pluto/diskless -a -d /usr/pluto/diskless ]; then
	mv /usr/pluto/diskless /home
fi
mkdir -p /home/diskless
ln -sf /home/diskless /usr/pluto/diskless

# update atftp entry in inet.d
update-inetd --remove tftp
update-inetd --group BOOT --add "tftp        dgram   udp wait    nobody /usr/sbin/tcpd /usr/sbin/in.tftpd --tftpd-timeout 300 --retry-timeout 5     --mcast-port 1758 --mcast-addr 239.255.0.0-255 --maxthread 100 --verbose=5 --no-blksize /tftpboot"

### Configure polipo, frox and apt
# /etc/polipo/config
polipo_conf='# Pluto config for polipo
proxyAddress = "::"
proxyPort = 8123
allowedClients = 0.0.0.0/0
logFile = /var/log/polipo.log
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
# /etc/apt/apt.conf.d/30pluto
pluto_apt_conf='// Pluto apt conf add-on
Acquire::http::Proxy "http://127.0.0.1:8123";
Acquire::ftp::Proxy "ftp://127.0.0.1:8124";
Acquire::ftp::ProxyLogin { "USER $(SITE_USER)@$(SITE):$(SITE_PORT)"; "PASS $(SITE_PASS)"; };
Acquire::http::Proxy::dcerouter "DIRECT";
Apt::Cache-Limit "12582912";
'

echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto
echo -n "$polipo_conf" >/etc/polipo/config
echo -n "$frox_conf" >/etc/frox.conf
/usr/sbin/adduser --system --group --home /var/cache/frox --disabled-login --disabled-password frox &>/dev/null|| /bin/true
echo "RUN_DAEMON=yes" >/etc/default/frox
/etc/init.d/frox restart
/etc/init.d/polipo restart
