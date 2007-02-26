#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

FindDeviceIP()
{
	Q="SELECT IPAddress FROM Device WHERE PK_Device='$1'"
	Row="$(RunSQL "$Q")"
	echo "$(Field 1 "$Row")"
}

FindPackageName()
{
	Q="SELECT PackageName FROM Software WHERE PK_Software='$1'"
	Row="$(RunSQL "$Q")"
	echo "$(Field 1 "$Row")"
}

InstallPackage()
{
#simply fetch and install it for now
}

Action=$1
DeviceID=$2
PackageID=$3

case $Action in
	install|remove)
		DeviceIP="$(FindDeviceIP "$DeviceID")"
		if [[ -z "$DeviceIP" ]]; then
			echo "IP for the device $DeviceID is not set"
			exit 1
		fi
	
		echo "Launching $Action on $DeviceIP"
		/usr/pluto/bin/LaunchRemoteCmd.sh "$DeviceIP" "/usr/pluto/bin/AddSoftwareHelper.sh $Action-actual $DeviceID $PackageID"
	;;
	
	
	install-actual)
		if [[ "$DISTRIB_ID" == "Ubuntu" ]] ;then
						Distro="edgy"
		else
						Distro="sarge"
		fi

		PackageName="$(FindPackageName "$PackageID")"
		
		#getting sources for packages
		Q="SELECT COUNT(*) FROM Software_Source WHERE Distro='$Distro' AND FK_Software=$PackageID AND Virus_Free=1"
		Row="$(RunSQL "$Q")"
		SourcesCount="$(Field 1 "$Row")"
		echo "Found $SourcesCount source for package $PackageName"
		
		for i in `seq 1 $SourcesCount`; do
			echo "Trying source $i from $SourcesCount"
			SourcesShift=$(( i - 1 ))
			Q="SELECT Downloadurl,Sum_md5,Sum_sha,PK_Software_Source FROM Software_Source WHERE Distro='$Distro' AND FK_Software=$PackageID AND Virus_Free=1 ORDER BY PK_Software_Source LIMIT $SourcesShift,1"
			Row="$(RunSQL "$Q")"
			DownloadURL="$(Field 1 "$Row")"
			MD5Sum="$(Field 2 "$Row")"
			SHA1Sum="$(Field 3 "$Row")"
			PK_Software_Source="$(Field 4 "$Row")"
			
			#Result="$(InstallPackage "$DownloadURL" "$MD5Sum" "$SHA1Sum")"
			wget -P /tmp "$DownloadURL"
			RetCode=$?
			if [ "$RetCode" -eq 0 ]; then
				WaitLock "InstallNewDevice" "AddSoftwareHelper"
		
				dpkg -i /tmp/$PackageName
				echo "($?) dpkg -i /tmp/$PackageName"
		
				Unlock "InstallNewDevice" "AddSoftwareHelper"
				Q="UPDATE Software_Device SET Status='Y', FK_Software_Source=$PK_Software_Source WHERE FK_Device=$DeviceID AND FK_Software_Source IN (SELECT PK_Software_Source FROM Software_Source WHERE FK_Software=$PackageID)"
				Row="$(RunSQL "$Q")"
				Q="INSERT INTO Software_Device(FK_Software_Source, Status, FK_Device) VALUES($PK_Software_Source, 'Y', $DeviceID)"
				break
			else
				Result="Download failed (direct link)"
			fi
			
		done

	;;

	remove-actual)
		# getting package name
		PackageName="$(FindPackageName "$PackageID")"
		if [[ -z "$PackageName" ]]; then
			echo "Package with ID=$PackageID is not found"
			exit 1
		fi
		
		echo "Removing package $PackageName"
		WaitLock "RemoveNewDevice"
		#dpkg --remove $PackageName
		Unlock "RemoveNewDevice"
		Q="UPDATE Software_Device SET Status='N' WHERE FK_Device=$DeviceID AND FK_Software_Source IN (SELECT PK_Software_Source FROM Software_Source WHERE FK_Software=$PackageID)"
		Row="$(RunSQL "$Q")"
	;;
	
	*)
		echo "Incorrect syntax, please use: $0 (install|install-actual|remove|remove-actual) DeviceID PackageID"
	;;
esac


