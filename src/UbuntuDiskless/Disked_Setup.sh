#!/bin/bash

set -e 

. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null


DEVICEDATA_DisklessBoot=9
DestDir=$(mktemp -d)

function update_config_files
{
	local ScriptDir="/usr/pluto/bin/files.d"
	local ScriptsList="cron.d-synctime interfaces mythtv-mysql.txt nis-client pluto.conf resolv.conf syslog.conf timezone mountnfs.sh apt.conf"
	for Script in $ScriptsList ;do
		if [[ ! -x $ScriptDir/$Script ]] ;then
			echo "WARNING: Script $Script cannot be executed"
			continue
		fi

		pushd $ScriptDir
			$ScriptDir/$Script --root $DestDir --device $Moon_DeviceID
		popd
	done
}

function build_installer_script 
{
	mkdir -p "${DestDir}/usr/pluto/install"
	local InstallerFiles="ConfirmDependencies_Debian.sh Common.sh"
	for file in $InstallerFiles ;do
		cp "/usr/pluto/install/${file}" "${DestDir}/usr/pluto/install"
	done

	/usr/pluto/bin/ConfirmDependencies -o 14 -r -D pluto_main -h dcerouter -u root -p '' -d $Moon_DeviceID install > "${DestDir}/usr/pluto/install/activation.sh"
}

function create_archive
{
	pushd ${DestDir}
		tar zcf /usr/pluto/var/Disked_${Moon_DeviceID}.tar.gz *
	popd
}


Q="
	SELECT 
		PK_Device, 
		IPaddress
	FROM 
		Device
		JOIN Device_DeviceData ON PK_Device = FK_Device AND FK_DeviceData = $DEVICEDATA_DisklessBoot
	WHERE 
		FK_DeviceTemplate = '28'
		AND
		FK_Device_ControlledVia IS NULL
		AND
		IK_DeviceData = '0'
"

R=$(RunSQL "$Q")
for Row in $R ;do
	Moon_DeviceID=$(Field "1" "$Row")
	Moon_IP=$(Field "2" "$Row");
	Moon_IP=$(/usr/pluto/bin/PlutoDHCP.sh -d "$Moon_DeviceID" -a)

	update_config_files
	build_installer_script
	create_archive
done

