#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func 

function printHelp()
{
	Logging "$TYPE" "$SEVERITY_NORMAL" "Invalid paramters: ./Start_LocalDevices.sh [ -d DeviceID [-r RouterAddress ] ]";
}

[ $# -ne 4 -a $# -ne 2 -a $# -ne 0 ] && printHelp && exit;
CurrentDevice=$PK_Device;

export DISPLAY=:0

[ $# -eq 2 -o $# -eq 4 ] && [ $1 != "-d" ] && printHelp && exit;
[ $# -eq 2 -o $# -eq 4 ] && CurrentDevice=$2;

[ $# -eq 4 ] && [ $3 != "-r" ] && printHelp && exit;
[ $# -eq 4 ] && DCERouter=$4;

Logging "$TYPE" "$SEVERITY_NORMAL" "Launching child devices for device: $CurrentDevice";
QUERY="SELECT PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine
		FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_Device_ControlledVia=$CurrentDevice AND FK_DeviceCategory <> 1";

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

	if [ ! -f /usr/pluto/bin/$ChildCommand ]; then 
		Logging "$TYPE" "$SEVERITY_WARNING" "Child device ($ChildDeviceID) was configured but the startup script ($ChildCommand) is not available in /usr/pluto/bin.";
	elif [ ! -x /usr/pluto/bin/$ChildCommand ]; then 
		Logging "$TYPE" "$SEVERITY_WARNING" "Child device ($ChildDeviceID) was configured but the startup script ($ChildCommand) existent in /usr/pluto/bin is not executable.";
	elif [ `basename $0` = `basename $ChildCommand` ]; then
		pushd /usr/pluto/bin
		/usr/pluto/bin/$ChildCommand -d $ChildDeviceID -r $DCERouter;
		popd
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "Launching device $ChildDeviceID in screen session ($ChildDescription)";
		pushd /usr/pluto/bin
		screen -d -m -S "$ChildDescription-$ChildDeviceID" /usr/pluto/bin/Spawn_Device.sh $ChildDeviceID $DCERouter $ChildCommand
		popd
	fi;
done
