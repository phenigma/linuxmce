#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

DISTRO_Debian=1
DISTRO_Ubuntu=14
REPOSITORYTYPE_Package=1


if [[ -r /etc/lsb-release ]] ;then
	. /etc/lsb-release
fi

if [[ "$DISTRIB_ID" == "Ubuntu" ]] ;then
	DISTRO=$DISTRO_Ubuntu
else
	DISTRO=$DISTRO_Debian
fi

# Get list of missing packages
Q="
	SELECT DeviceTemplate.FK_Package
	FROM Device
	JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
	LEFT JOIN Package_Device ON Package_Device.FK_Package=DeviceTemplate.FK_Package AND Package_Device.FK_Device=$PK_Device
	LEFT JOIN Device AS P1 on Device.FK_Device_ControlledVia=P1.PK_Device
	LEFT JOIN Device AS P2 on P1.FK_Device_ControlledVia=P2.PK_Device
	LEFT JOIN Device AS P3 on P2.FK_Device_ControlledVia=P3.PK_Device
	LEFT JOIN Device AS P4 on P3.FK_Device_ControlledVia=P4.PK_Device
	LEFT JOIN Device AS P5 on P4.FK_Device_ControlledVia=P5.PK_Device
	WHERE Package_Device.FK_Package IS NULL AND DeviceTemplate.FK_Package IS NOT NULL AND
	(
		Device.FK_Device_ControlledVia=$PK_Device
		OR P1.FK_Device_ControlledVia=$PK_Device
		OR P2.FK_Device_ControlledVia=$PK_Device
		OR P3.FK_Device_ControlledVia=$PK_Device
		OR P4.FK_Device_ControlledVia=$PK_Device
		OR P5.FK_Device_ControlledVia=$PK_Device
	)
"
PackageIDs=$(RunSQL "$Q")

if [[ -z "$PackageIDs" ]]; then
	exit # no packages to install
fi

Q="
	SELECT ps.Name,rsu.URL,ps.Repository
	FROM Package_Source ps
	JOIN Package_Source_Compat psc ON ps.PK_Package_Source=psc.FK_Package_Source AND psc.FK_Distro=$DISTRO
	JOIN RepositorySource rs ON ps.FK_RepositorySource=rs.PK_RepositorySource AND FK_RepositoryType=$REPOSITORYTYPE_Package
	JOIN RepositorySource_URL rsu ON ps.FK_RepositorySource=rsu.FK_RepositorySource
	WHERE FK_Package IN (${PackageIDs// /,})
"
PackageDataRows=$(RunSQL "$Q")

for Row in $PackageDataRows; do
	PkgName="$(Field 1 "$Row")"
	PkgURL="$(Field 2 "$Row")"
	PkgRepository="$(Field 3 "$Row")"
	echo "Installing package: '$PkgName'; URL: '$PkgURL'; Repository: '$PkgRepository'"
	/usr/pluto/bin/InstallSoftware.sh 127.0.0.1 "$PkgName" "$PkgURL" "$PkgRepository"
done
