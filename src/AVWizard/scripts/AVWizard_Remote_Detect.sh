#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/AVWizard-Common.sh

PipeField()
{
	local Field="$1" local Data="$2"

	echo "$Data" | cut -d'|' -f"$Field"
}

Q="
	SELECT VendorModelID,PK_DeviceTemplate,DeviceTemplate.Description,CommandLine,Name
	FROM DHCPDevice
	JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
	LEFT JOIN Package_Source ON DeviceTemplate.FK_Package=Package_Source.FK_Package AND FK_RepositorySource=2
	WHERE DeviceTemplate.FK_DeviceCategory=132
"
R=$(RunSQL "$Q")

OutFile=/tmp/device_find.txt
rm -f "$OutFile"
for Row in $R; do
	VendorModelID=$(Field 1 "$Row")
	PK_DeviceTemplate=$(Field 2 "$Row")
	DTDescription=$(Field 3 "$Row")
	CmdName=$(Field 4 "$Row")
	Name=$(Field 5 "$Row")
	echo "$VendorModelID|$PK_DeviceTemplate|$DTDescription|$CmdName|$Name" >>"$OutFile"
done

Remote=$(/usr/pluto/bin/hal_device_finder -f "$OutFile")
RemotePort=$(PipeField 1 "$Remote")
RemoteDT=$(PipeField 2 "$Remote")
RemoteDescription=$(PipeField 3 "$Remote")
RemotePkg=$(PipeField 4 "$Remote")

if ! PackageIsInstalled "$RemotePkg"; then
	apt-get -y install "$RemotePkg"
fi

PerlCommand="
	chomp;
	\$desc = \$_;
	\$desc =~ s/[ :+=()<]/_/g;
	\$desc =~ s/[->*?\\$\.%\\/]//g;
	\$desc =~ s/#/Num/g;
	\$desc =~ s/__/_/g;
	\$desc =~ s/^_*//g;
	\$desc =~ s/_*\$//g;
	print \"\$desc\n\";
"

Q="
	SELECT CommandLine,Description
	FROM DeviceTemplate
	WHERE PK_DeviceTemplate='$RemoteDT'
"
R=$(RunSQL "$Q")
CommandLine=$(Field 1 "$R")
Description=$(Field 2 "$R")

if [[ -z "$CommandLine" ]]; then
	CommandLine=$(echo "$Description" | perl -n -e "$PerlCommand")
fi

# Last line of output is our result
if [[ -n "$RemotePort" ]]; then
	echo "/usr/pluto/bin/$CommandLine -p $RemotePort -l /var/log/pluto/avremote.log -d -1003 -H $DCERouter -P $AVWizard_Port"
else
	echo "" # no remote
fi
