#!/bin/bash

# Temp code
echo "About to start. Press Enter."
read

Sources="# Pluto sources - start
deb file:/usr/pluto/deb-cache/ sarge main
deb http://deb.plutohome.com/debian/ sarge main non-free contrib
deb http://deb.plutohome.com/debian/ unstable mythtv
deb http://www.yttron.as.ro/ sarge main
# Pluto sources - end"

PlutoConf="# Pluto config file
MySqlHost = localhost
MySqlUser = root
MySqlPassword =
MySqlDBName = pluto_main
DCERouter = localhost
MySqlPort = 3306
DCERouterPort = 3450
PK_Device = 1
Activation_Code =
PK_Installation = 1
PK_Users = 1"

Ask()
{
	local Msg="$1"
	echo -n "$Msg: " >/dev/tty
	read Answer
	echo "$Answer"
}

# Ask the questions
Username=$(Ask "Pluto Username")
Password=$(Ask "Pluto Password")
Room=$(Ask "Room name")
Type=$(Ask "Core or Hybrid? [C/h]")

echo "$PlutoConf" >/etc/pluto.conf

echo "$Sources" >/etc/apt/sources.list
aptitude update
if ! aptitude -y -f install pluto-dcerouter; then
	echo "Installation failed"
	exit 1
fi

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# "Initial config"
# Create installation
Q="INSERT INTO Installation VALUES()"
RunSQL "$Q"

# Create user
PasswordUnix=$(mkpasswd -H md5 "$Password")
PasswordSamba=$(/usr/pluto/bin/smbpass.pl "$Password")

sqlUsername="${Username//\'/\'}"
sqlPassword="${Password//\'/\'}"
sqlPasswordUnix="${PasswordUnix//\'/\'}"
sqlPasswordSamba="${PasswordSamba//\'/\'}"
Q="INSERT INTO Users(UserName, Password, Password_Unix, Password_Samba)
	VALUES('$sqlUsername', MD5('$sqlPassword'), '$sqlPasswordUnix', '$sqlPasswordSamba')"
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
CoreDev=$(/usr/pluto/bin/CreateDevice -d 7)
Q="UPDATE Device SET Description='CORE', FK_Room=1 WHERE PK_Device='$CoreDev'"
RunSQL "$Q"

# Add Core options
Q='SELECT FK_DeviceTemplate FROM InstallWizard WHERE `Default`=1 AND Step=5'
R=$(RunSQL "$Q")

for DevT in $R; do
	Dev=$(/usr/pluto/bin/CreateDevice -d "$DevT")
done

# Create hybrid
if [[ "$Type" == "H" || "$Type" == "h" ]]; then
	HybDev=$(/usr/pluto/bin/CreateDevice -d 28 -C "$CoreDev")
	Q="UPDATE Device SET Description='The core/hybrid', FK_Room=1 WHERE PK_Device='$HybDev'"
	RunSQL "$Q"

	# Add hybrid options
	Q='SELECT FK_DeviceTemplate FROM InstallWizard WHERE `Default`=1 AND Step=6'
	R=$(RunSQL "$Q")

	for DevT in $R; do
		Dev=$(/usr/pluto/bin/CreateDevice -d "$DevT")
	done
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

# Add to Installation_Users
Q4="INSERT INTO Installation_Users(FK_Installation,FK_Users,userCanModifyInstallation)
SELECT PK_Installation,PK_Users,1
FROM Installation
JOIN Users;"

echo "$Q4" | /usr/bin/mysql pluto_main

# Update startup scripts
/usr/pluto/bin/Update_StartupScrips.sh

# Remove installation incomplete flag
rm /usr/pluto/install/.notdone

# Set vimrc
[[ -e /root/.vimrc ]] || cp /usr/share/vim/vim63/vimrc_example.vim /root/.vimrc

# Set debconf frontend
awk '/Name: debconf\/frontend/,/^$/ {if ($1 == "Value:") print "Value: Noninteractive"; else print; next}
        {print}' /var/cache/debconf/config.dat > /var/cache/debconf/config.dat.$$
mv /var/cache/debconf/config.dat{.$$,}

exit 0
# /Temp code

# router/diskless
Type="router"

mkdir -p /var/log/pluto
File="/var/log/pluto/Initial_Install.newlog"
: >"$File"
exec 3>&1 4>&2
bash -x /usr/pluto/install/Initial_Config_Real.sh "$Type" > >(tee -a "$File" >&3) 2> >(tee -a "$File" | grep -v '^++* ' >&4)
sleep 1 # so tee and grep close and flush their buffers
exec 3>&- 4>&-
