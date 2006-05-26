#!/bin/bash

. /usr/pluto/install/Common.sh
DIR="/usr/pluto/install"

mkdir -p /home/pluto/logs
mkdir -p /home/public/data

Sources="# Pluto sources - start
deb file:/usr/pluto/deb-cache/ sarge main
deb http://deb.plutohome.com/debian/ 20dev main
deb http://deb.plutohome.com/debian/ replacements main
deb http://deb.plutohome.com/debian/ sarge main non-free contrib
deb http://deb.plutohome.com/debian/ unstable mythtv
deb http://www.yttron.as.ro/ sarge main
# Pluto sources - end"
SourcesOffline="# Pluto sources offline - start
deb file:/usr/pluto/deb-cache/ sarge main
# Pluto sources offline - end"
echo "$Sources" >/etc/apt/sources.list
echo "$SourcesOffline" >/etc/apt/sources.list.offline
sleep 0.5
clear

exec 3>&1 1>/dev/tty
clear
MessageBox "
I need to ask some basic questions.

Please see http://plutohome.com/support, click 
Quick Start Guide, and Installing Software for details.

"

# Ask the questions
Username=
while [[ $Username == "" ]] ;do
	Username=$(InputBox "You need a username and a password for accesing the pluto-admin site.	

What username do you want for yourself?")
done

ConfPassword=
Password=
while [[ $ConfPassword != $Password || $Password == "" ]] ;do

	while [[ $Password == "" ]] ;do
		Password=$(PasswordBox "Password for user $Username")
	done
	ConfPassword=$(PasswordBox "Confirm password for $Username")

	if [[ $Password != $ConfPassword ]] ;then
		MessageBox "Passwords do not match !" --title Error
		Password=
	fi
done

Room=$(InputBox "In what room is this PC?")

Type=$(whiptail --menu "
Hybrid means this PC will not only be the main server, but you will also hook it up to a TV and use it as a media director.
Core means it is a server only, and will not be used as a media station.

Should this be a Core, Hybrid or Other?
" 0 0 3 C - Core H - Hybrid o - Other --fb --nocancel --title Pluto 2>&1 1>/dev/tty)

CoreDT=7
if [[ "$Type" == o || "$Type" == O ]]; then
	CoreDT=$(InputBox "Enter the device template number")
fi


UseInternet=$(QuestionBox "If you have an active internet connection, Pluto can use it to perform
various task : syncronize your computer clock, automaticaly send bug
reports, install aditional software, perform updates ...

You can also switch from online/offline mode later by using the web
administration interface

Important : Keep in mind that when you don't have an active internet
connection, this tasks can slow down your computer and an negative
answer here would be the right choice.

Should Pluto use your internet connection ?")

DHCP=$(QuestionBox "You need to answer Yes below if you want Plug-and-Play or extra media directors.

Do you want to run a DHCP server ?
")

RestoreCoreConf()
{
	[[ -d /home/backup ]] || return 0
	[[ -f /home/backup/pluto.conf-Core ]] || return 0

	sed "$NoSpace" /home/backup/pluto.conf-Core | egrep -v "^#|^//" >/etc/pluto.conf
	while read line; do
		eval "local $line" &>/dev/null
	done </etc/pluto.conf
	export Device="$PK_Device"
	export Code="$Activation_Code"
}

# /etc/apt/apt.conf.d/30pluto
pluto_apt_conf='// Pluto apt conf add-on
//Acquire::http::Proxy "http://127.0.0.1:8123";
//Acquire::ftp::Proxy "ftp://127.0.0.1:8124";
//Acquire::ftp::ProxyLogin { "USER $(SITE_USER)@$(SITE):$(SITE_PORT)"; "PASS $(SITE_PASS)"; };
//Acquire::http::Proxy::dcerouter "DIRECT";
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
Acquire::http::timeout "10";
Acquire::ftp::timeout "10";
'

# Install critical (Deb talk: essential) packages
CriticalPattern="/usr/pluto/install/deb-critical/*"
CriticalPkgs=$(echo $CriticalPattern)
if [[ -n "$CriticalPkgs" ]]; then
	if dpkg -i $CriticalPattern; then
		echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto
		echo -n "$polipo_conf" >/etc/polipo/config
		echo -n "$frox_conf" >/etc/frox.conf
		/usr/sbin/adduser --system --group --home /var/cache/frox --disabled-login --disabled-password frox
		echo "RUN_DAEMON=yes" >/etc/default/frox
		/etc/init.d/frox restart
		/etc/init.d/polipo restart
	else
		echo "* ERROR * Complete proxy combination is not available. Installation can't continue"
		read
		exit 1
	fi
fi
RestoreCoreConf
WhereCode="in pluto.conf backup"

# Pluto installation
Activation_Code=1111

OfflineMode="false"
if [[ "$UseInternet" == "N" || "$UseInternet" == "n" ]] ;then
	OfflineMode="true"
        OfflineApt='
Acquire::http::timeout "-1";
Acquire::ftp::timeout "-1";
APT::Get::Download "false";
APT::Get::Fix-Missing "true";
APT::Get::quiet "1";
Dir::Etc::sourcelist "sources.list.offline";
'
        mkdir -p /etc/apt/apt.conf.d/
        ln -s /usr/pluto/var/apt.conf.offline /etc/apt/apt.conf.d/99offline
	mkdir -p /usr/pluto/var
        echo -e $OfflineApt > /usr/pluto/var/apt.conf.offline

fi

PlutoConf="# Pluto config file
MySqlHost = localhost
MySqlUser = root
MySqlPassword =
MySqlDBName = pluto_main
DCERouter = localhost
MySqlPort = 3306
DCERouterPort = 3450
PK_Device = 1
Activation_Code = $Activation_Code
PK_Installation = 1
PK_Users = 1
OfflineMode = $OfflineMode
"

echo "$PlutoConf" >/etc/pluto.conf

apt-get update
if ! apt-get -y -f install pluto-dcerouter; then
	echo "Installation failed"
	exit 1
fi

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# "Initial config"
# Create installation
Q="INSERT INTO Installation(Description, ActivationCode) VALUES('Pluto', '$Activation_Code')"
RunSQL "$Q"

# Create user
PasswordUnix=$(mkpasswd -H md5 "$Password")
PasswordSamba=$(/usr/pluto/bin/smbpass.pl "$Password")

sqlUsername="${Username//\'/\'}"
sqlPassword="${Password//\'/\'}"
sqlPasswordUnix="${PasswordUnix//\'/\'}"
sqlPasswordSamba="${PasswordSamba//\'/\'}"
Q="INSERT INTO Users(UserName, Password, Password_Unix, Password_Samba, Extension)
	VALUES('$sqlUsername', MD5('$sqlPassword'), '$sqlPasswordUnix', '$sqlPasswordSamba', 301)"
RunSQL "$Q"

# Add user to installation
Q="INSERT INTO Installation_Users(FK_Installation, FK_Users, userCanModifyInstallation)
	VALUES(1, 1, 1)"
RunSQL "$Q"

# Create room
sqlRoom="${Room//\'/\'}"
Q="INSERT INTO Room(FK_Installation, FK_RoomType, Description)
	VALUES(1, 9, '$sqlRoom')"
RunSQL "$Q"

# Add Core
CoreDev=$(NewDev -d "$CoreDT")
Q="UPDATE Device SET Description='CORE', FK_Room=1 WHERE PK_Device='$CoreDev'"
RunSQL "$Q"

# Create hybrid
if [[ "$Type" == "H" || "$Type" == "h" ]]; then
	HybDev=$(NewDev -d 28 -C "$CoreDev")
	Q="UPDATE Device SET Description='The core/hybrid', FK_Room=1 WHERE PK_Device='$HybDev'"
	RunSQL "$Q"
fi

DHCPsetting=
if [[ "$DHCP" != n && "$DHCP" != N ]]; then
	DHCPsetting="192.168.80.2-192.168.80.128,192.168.80.129-192.168.80.254"
fi

if [[ -n "$DHCPsetting" ]]; then
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES($CoreDev, 28, '$DHCPsetting')"
	RunSQL "$Q"
fi

# "DCERouter postinstall"
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

echo "$Q4" | /usr/bin/mysql pluto_main

# Update startup scripts
/usr/pluto/bin/Update_StartupScrips.sh

# Remove installation incomplete flag
rm /usr/pluto/install/.notdone

selectedInterface=$(grep 'iface..*eth' /etc/network/interfaces | awk '{print $2}')
dcerouterIP=$(ifconfig $selectedInterface | awk 'NR==2' | cut -d: -f2 | cut -d' ' -f1)

hosts="
127.0.0.1       localhost.localdomain   localhost
$dcerouterIP    dcerouter $(/bin/hostname)

# The following lines are desirable for IPv6 capable hosts
::1     ip6-localhost ip6-loopback
fe00::0 ip6-localnet
ff00::0 ip6-mcastprefix
ff02::1 ip6-allnodes
ff02::2 ip6-allrouters
ff02::3 ip6-allhosts
"
echo "$hosts" >/etc/hosts

clear

if [[ -d /home/backup && -f /home/backup/entire_database.sql ]]; then
	echo "Restoring database"
	mysql < /home/backup/entire_database.sql
fi

/usr/pluto/bin/Network_Setup.sh
/usr/pluto/bin/DHCP_config.sh

clear
# XXX: No error checking
	MessageBox "The Pluto Bonus CD version 1 has some extras, such as a video setup wizard to help \nget you up and running. If you have the Pluto Bonus CD version 1, \nplease insert it into your drive now and choose Y after it is in. Otherwise, choose N."
        echo ""
        BonusCD=$(QuestionBox "Did you insert the \"Pluto Bonus CD 1\" in drive ?")
	if [[ "$BonusCD" == Y || "$BonusCD" == y ]]; then

                /bin/mount /dev/cdrom 2>/dev/null
                
		        while [ ! -d "/cdrom/bonuscd1" ]; do
                        	MessageBox "This in not a valid \"Pluto Bonus CD 1\". Please insert the correct CD and press any key to continue"
	                        /usr/bin/eject
                	        read key
                                if [[ ! -n "$key" ]]; then
                                        /bin/mount /dev/cdrom 2>/dev/null
                                fi
                        done

                echo "Installing extra packages from \"Pluto Bonus CD 1\""
                echo "... PLEASE WAIT ..."

		for files in $(ls /cdrom/bonuscd1); do
		pkgname=$(echo $files | awk -F '_' '{print $1}')
		pkginstalled=$(dpkg -l $pkgname 1>/dev/null)
		        if [[ -n "$pkginstalled" ]]; then
		                echo "Package $files is allready installed ... skipping ..."
		        else
                		echo "Installing package name : $files"
		                cd /cdrom/bonuscd1
                		dpkg -i $files 1>/dev/null
		        fi
		done

		cd /cdrom/bonuscd1-cache
                cp -r *.deb /usr/pluto/deb-cache/dists/sarge/main/binary-i386/
                cd /usr/pluto/deb-cache/dists/sarge/main/binary-i386/
                /usr/bin/screen -A -m -d -S pkgsrebuild dpkg-scanpackages . /dev/null | sed 's,\./,dists/replacements/main/binary-i386/,g' | gzip -9c > Packages.gz
                cd ..
                /usr/bin/eject
                echo ""
                MessageBox "\"Pluto Bonus CD 1\" succesfuly installed !"
                echo ""
        else
                MessageBox "Skipping \"Pluto Bonus CD 1\" install ..."
        fi


while :; do
	ExtraPkg=$(QuestionBox "Do you want to add extra packages?")
	if [[ "$ExtraPkg" == y || "$ExtraPkg" == Y ]]; then
		while :; do
			ExtraRepository=$(QuestionBox "Add a new repository? ")
			if [[ "$ExtraRepository" == y || "$ExtraRepository" == Y ]]; then
				ExtraRepositoryPath=$(InputBox "Enter repository path")
				echo "$ExtraRepositoryPath" >>/etc/apt/sources.list
				apt-get update
				break
			elif [[ "$ExtraRepository" == n || "$ExtraRepository" == N ]]; then
				break
			else
				MessageBox "--> Invalid answer: '$ExtraRepository'"
			fi
		done
		ExtraPkgName=$(InputBox "Enter package name")
		apt-get install "$ExtraPkgName"
	elif [[ "$ExtraPkg" == n || "$ExtraPkg" == N ]]; then
		break
	else
		MessageBox "--> Invalid answer: '$ExtraPkg'"
	fi
done
exec 1>&3 3>&-
# XXX: No error checking

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/generateRcScripts.sh

sed -i 's/^ide-generic$/#&/g' /etc/modules

/usr/pluto/install/Initial_Config_Finish.sh

#init q
exit 0
