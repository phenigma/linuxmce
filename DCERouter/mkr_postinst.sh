#enable MySQL networking
sed -i 's/^skip-networking/#skip-networking/' /etc/mysql/my.cnf
/etc/init.d/mysql restart

echo "setting up dce router2"
hasRecords=`echo 'SELECT count(PK_Installation) FROM Installation' | mysql pluto_main | tail -n 1`;
if [ $hasRecords -ne 0 ]; then 
	echo "Database already setup";
	exit 0
fi

. /usr/pluto/bin/Config_Ops.sh

device="$PK_Device"
code="$Activation_Code"

wget -O /usr/pluto/install/database_initial_data.sql "http://plutohome.com/initialdata.php?code=$code&device=$device" 2>/dev/null

size=$(stat -c "%s" /usr/pluto/install/database_initial_data.sql)
if [ "$size" -lt 100 ]; then
	echo "Error getting your initial data."
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

(echo "$Q4";) | /usr/bin/mysql pluto_main

/usr/pluto/bin/Update_StartupScrips.sh
mkdir -p /tftpboot/pxelinux.cfg
cp /usr/lib/syslinux/pxelinux.0 /tftpboot
