#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func 

function printHelp()
{
	Logging "$TYPE" "$SEVERITY_NORMAL" "Invalid paramters: ./Start_LocalDevices.sh [ -d DeviceID [-r RouterAddress ] ]" >&2;
}

Alternative=/usr/pluto/bin/Start_LocalDevices_Static.sh

if [ -f "$Alternative" -a -x "$Alternative" ]; then
	"$Alternative"
	exit $?
fi

if [ "$1" == "script" ]; then
	Script="script"; MDIP="$2";
	shift; shift;
	Path="/usr/pluto/diskless/$MDIP/"
	
	echo ". /usr/pluto/bin/Config_Ops.sh"
	echo ". /usr/pluto/bin/pluto.func"
fi

[ $# -ne 4 -a $# -ne 2 -a $# -ne 0 ] && printHelp && exit;
CurrentDevice=$PK_Device;

export DISPLAY=:0

[ $# -eq 2 -o $# -eq 4 ] && [ $1 != "-d" ] && printHelp && exit;
[ $# -eq 2 -o $# -eq 4 ] && CurrentDevice=$2;

[ $# -eq 4 ] && [ $3 != "-r" ] && printHelp && exit;
[ $# -eq 4 ] && DCERouter=$4;

if [ -z "$Script" ]; then
	Logging "$TYPE" "$SEVERITY_NORMAL" "Launching child devices for device: $CurrentDevice";
else
	echo 'Logging "$TYPE" "$SEVERITY_NORMAL" "Launching child devices for device: '"$CurrentDevice"'";'
fi
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

CommandList=$(echo "$QUERY" | $MySQLCommand | tail +2 | perl -n -e "$PerlCommand");
for command in $CommandList; do
	ChildDeviceID=`echo $command | cut -f 1 -d '|'`;
	ChildCommand=`echo $command | cut -f 2 -d '|'`;
	ChildDescription=`echo $command | cut -f 3 -d '|'`;

	if [ ! -f "$Path/usr/pluto/bin/$ChildCommand" ]; then
		if [ -z "$Script" ]; then
			Logging "$TYPE" "$SEVERITY_WARNING" "Child device ($ChildDeviceID) was configured but the startup script ($ChildCommand) is not available in /usr/pluto/bin.";
		else
			echo 'Logging "$TYPE" "$SEVERITY_WARNING" "Child device ('"$ChildDeviceID"') was configured but the startup script ('"$ChildCommand"') is not available in /usr/pluto/bin.";'
		fi
	elif [ ! -x "$Path/usr/pluto/bin/$ChildCommand" ]; then
		if [ -z "$Script" ]; then
			Logging "$TYPE" "$SEVERITY_WARNING" "Child device ($ChildDeviceID) was configured but the startup script ($ChildCommand) existent in /usr/pluto/bin is not executable.";
		else
			echo 'Logging "$TYPE" "$SEVERITY_WARNING" "Child device ('"$ChildDeviceID"') was configured but the startup script ('"$ChildCommand"') existent in /usr/pluto/bin is not executable.";'
		fi
	elif [ `basename $0` = `basename $ChildCommand` ]; then
		pushd /usr/pluto/bin >/dev/null
		if [ -z "$Script" ]; then
			/usr/pluto/bin/$ChildCommand -d $ChildDeviceID -r $DCERouter;
		else
			/usr/pluto/bin/$ChildCommand script "$MDIP" -d $ChildDeviceID -r $DCERouter;
		fi
		popd >/dev/null
	else
		if [ -z "$Script" ]; then
			Logging "$TYPE" "$SEVERITY_NORMAL" "Launching device $ChildDeviceID in screen session ($ChildDescription)";
			pushd /usr/pluto/bin >/dev/null
			screen -d -m -S "$ChildDescription-$ChildDeviceID" /usr/pluto/bin/Spawn_Device.sh $ChildDeviceID $DCERouter $ChildCommand
			popd >/dev/null
		else
			echo 'Logging "$TYPE" "$SEVERITY_NORMAL" "Launching device '"$ChildDeviceID"' in screen session ('"$ChildDescription"')";
			pushd /usr/pluto/bin
			screen -d -m -S "'"$ChildDescription-$ChildDeviceID"'" /usr/pluto/bin/Spawn_Device.sh '"$ChildDeviceID" "$DCERouter" "$ChildCommand"'
			popd'
		fi
	fi;
done
