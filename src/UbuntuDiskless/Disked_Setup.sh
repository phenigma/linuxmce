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
	local InstallerFiles="ConfirmDependencies_Debian.sh Common.sh AptSources.sh"
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

function setup_mysql_access 
{
	echo "* Setting up MySQL access for MD #${Moon_DeviceID}"
	RunSQL "GRANT ALL PRIVILEGES ON *.* TO 'root'@$Moon_IP; GRANT ALL PRIVILEGES ON *.* TO 'eib'@$Moon_IP"
	RunSQL "FLUSH PRIVILEGES"
}

function setup_hosts_file 
{
	echo "* Setting up /etc/hosts"
	local Content=""
	local Q="
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
			IK_DeviceData = '1'
	"
	local R=$(RunSQL "$Q")
	local Row
	for Row in $R ;do
		local DeviceID=$(Field 1 "$Row")
		local IP=$(Field 2 "$Row")

		if [[ "$IP" == "" ]] ;then
			continue
		fi

		Content="${Content}${IP}		moon${DeviceID}\n"
	done
	
	PopulateSection "/etc/hosts" "DiskledMD" "$Content"

	## Export hosts file to other computer
	echo | /usr/lib/yp/ypinit -m

	echo "---$Content---"
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
	if [[ -z "$Moon_IP" ]]; then
		echo "WARNING : No free IP left to assign for moon$Moon_DeviceID"
		continue
	fi

	update_config_files
	build_installer_script
	create_archive
	setup_mysql_access
done

setup_hosts_file
/usr/pluto/bin/Diskless_ExportsNFS.sh
