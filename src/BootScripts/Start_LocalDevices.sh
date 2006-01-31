#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func 

function printHelp()
{
	Logging "$TYPE" "$SEVERITY_NORMAL" "Invalid paramters: ./Start_LocalDevices.sh [ -d DeviceID [-r RouterAddress ] ]" >&2
}

/usr/pluto/bin/UpdateAvailableSerialPorts.sh

[[ $# -ne 4 && $# -ne 2 && $# -ne 0 ]] && printHelp && exit
CurrentDevice=$PK_Device

export DISPLAY=:0

[[ $# -eq 2 || $# -eq 4 && "$1" != "-d" ]] && printHelp && exit
[[ $# -eq 2 || $# -eq 4 ]] && CurrentDevice="$2"

[[ $# -eq 4 && "$3" != "-r" ]] && printHelp && exit
[[ $# -eq 4 ]] && DCERouter=$4

Logging "$TYPE" "$SEVERITY_NORMAL" "Launching child devices for device: $CurrentDevice"

QUERY="SELECT Device.PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine
	FROM Device
	JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate
	LEFT JOIN Device AS Device_Parent on Device.FK_Device_ControlledVia=Device_Parent.PK_Device
	LEFT JOIN DeviceTemplate AS DeviceTemplate_Parent ON Device_Parent.FK_DeviceTemplate=DeviceTemplate_Parent.PK_DeviceTemplate
	WHERE (Device.FK_Device_ControlledVia=$CurrentDevice 
	OR (Device_Parent.FK_Device_ControlledVia=$CurrentDevice AND DeviceTemplate_Parent.FK_DeviceCategory IN (6,7,8) ) )
	AND DeviceTemplate.FK_DeviceCategory <> 1";

MySQLCommand="mysql -h $MySqlHost -D $MySqlDBName"

PerlCommand="
	chomp;
	(\$dev, \$desc, \$command) = split('\t');
	\$desc =~ s/[ :+=()<]/_/g;
	\$desc =~ s/[->*?\\$\.%\\/]//g;
	\$desc =~ s/#/Num/g;
	\$desc =~ s/__/_/g;
	\$desc =~ s/^_*//g;
	\$desc =~ s/_*\$//g;

	if ( ! \$command || \$command eq \"NULL\" )
	{
		\$command = \$desc;
	}
	print \"\$dev|\$command|\$desc\n\";
";

CommandList=$(echo "$QUERY" | $MySQLCommand | tail +2 | perl -n -e "$PerlCommand" | tr '\n' ' ');
basename=$(basename $0);
Logging "$TYPE" "$SEVERITY_WARNING" "$basename" "Start_LocalDevices $*; CommandList: $CommandList";
for command in $CommandList; do
	ChildDeviceID=`echo $command | cut -f 1 -d '|'`;
	ChildCommand=`echo $command | cut -f 2 -d '|'`;
	ChildDescription=`echo $command | cut -f 3 -d '|'`;

	if [[ ! -f "$Path/usr/pluto/bin/$ChildCommand" ]]; then
		Logging "$TYPE" "$SEVERITY_WARNING" "$basename" "Child device ($ChildDeviceID) was configured but the startup script ($ChildCommand) is not available in /usr/pluto/bin.";
	elif [[ ! -x "$Path/usr/pluto/bin/$ChildCommand" ]]; then
		Logging "$TYPE" "$SEVERITY_WARNING" "$basename" "Child device ($ChildDeviceID) was configured but the startup script ($ChildCommand) existent in /usr/pluto/bin is not executable.";
	elif [[ "$basename" = `basename $ChildCommand` ]]; then
		pushd /usr/pluto/bin >/dev/null
		/usr/pluto/bin/$ChildCommand -d $ChildDeviceID -r $DCERouter;
		popd >/dev/null
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$basename" "Launching device $ChildDeviceID in screen session ($ChildDescription)";
		pushd /usr/pluto/bin >/dev/null
		screen -d -m -S "$ChildDescription-$ChildDeviceID" /usr/pluto/bin/Spawn_Device.sh $ChildDeviceID $DCERouter $ChildCommand
		popd >/dev/null
	fi;
done
