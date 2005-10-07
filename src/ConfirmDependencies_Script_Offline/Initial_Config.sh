#!/bin/bash

# Temp code
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

echo "$PlutoConf" >/etc/pluto.conf

echo "$Sources" >/etc/apt/sources.list
aptitude update
if ! aptitude install pluto-dcerouter; then
	echo "Installation failed"
	exit 1
fi

Ask()
{
	local Msg="$1"
	echo -n "$Msg: "
	read Answer
}

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# Create installation
Q="INSERT INTO Installation VALUES()"
RunSQL "$Q"

# Create user
Username=$(Ask "Pluto Username")
Password=$(Ask "Pluto Password")
PasswordUnix=$(mkpasswd -H md5 "$Password")
PasswordSamba=$(/usr/pluto/bin/smbpass.pl "$Password")

sqlUsername="${Username//\'/\'}"
sqlPassword="${Password//\'/\'}"
sqlPasswordUnix="${PasswordUnix//\'/\'}"
sqlPasswordSamba="${PasswordSamba//\'/\'}"
Q="INSERT INTO Users(UserName, Password, Password_Unix, Password_Samba)
	VALUES('$sqlUN', '$sqlPassword', '$sqlPasswordUnix', '$sqlPasswordSamba')"
RunSQL "$Q"

# Add user to installation
Q="INSERT INTO Installation_Users(FK_Installation, FK_Users, userCanModifyInstallation)
	VALUES(1, 1, 1)"
RunSQL "$Q"

# Create room
Room=$(Ask "Room name")

sqlRoom="${Room//\'/\'}"
Q="INSERT INTO Room(FK_Installation, FK_RoomType, Description)
	VALUES(1, 9, '$sqlRoom')"
RunSQL "$Q"

# Add Core
CoreDev=$(/usr/pluto/bin/CreateDevice -d 7)
Q="UPDATE Device SET Description='CORE', Room=1 WHERE PK_Device='$CoreDev'"
RunSQL "$Q"

# Add Core options
Q='SELECT FK_DeviceTemplate FROM InstallWizard WHERE `Default`=1 AND Step=5'
R=$(RunSQL "$Q")

for DevT in $R; do
	Dev=$(/usr/pluto/bin/CreateDevice -d "$DevT")
done

# Create hybrid
Type=$(Ask "Core or Hybrid? [C/h]")
if [[ "$Type" == "H" || "$Type" == "h" ]]; then
	HybDev=$(/usr/pluto/bin/CreateDevice -d 28)
	Q="UPDATE Device SET Description='The core/hybrid', Room=1 WHERE PK_Device='$HybDev'"
	RunSQL "$Q"

	# Add hybrid options
	Q='SELECT FK_DeviceTemplate FROM InstallWizard WHERE `Default`=1 AND Step=6'
	R=$(RunSQL "$Q")

	for DevT in $R; do
		Dev=$(/usr/pluto/bin/CreateDevice -d "$DevT")
	done
fi

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

# Start Pluto
StartNow=$(Ask "Reboot? [y/N]")
if [[ "$StartNow" == "y" || "$StartNow" == "Y" ]]; then
	reboot
else
	/etc/init.d/Startup_Scripts.sh start
fi

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
