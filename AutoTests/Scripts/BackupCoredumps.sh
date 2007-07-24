#!/bin/bash

touch /var/log/BackupCoredumps.log
exec 6>&1 6>&2
exec > /var/log/BackupCoredumps.log 2>&1


PATH=$PATH:/usr/pluto/bin

coreCount=$(ls /usr/pluto/coredump/* |  wc -l)
binFiles=`ls /usr/pluto/coredump/* | sed 's%^/usr/pluto/coredump/core_%%' | sed 's/_[0-9]*_.*$//' | sort -u`
binCount=$(echo $binFiles | wc -w )

if [[ "$coreCount" == "0" ]]; then
	echo "There are no coredumps today."
else
	echo "There are $coreCount coredumps for $binCount binaries: "
fi

## Install smbfs as is not installed
#pt-get -y install smbfs


MountDir=/coredumps-`date +%s`
mkdir $MountDir
mount -t smbfs -o password=foo //10.0.0.150/coredumps $MountDir
ipThis=`ssh dcerouter ifconfig | sed -e 's/.*inet addr:\(10\.0\.0\.[0-9]*\).*/\1/;t;d;'`
#if [[ -f /etc/diskless.conf ]] ;then
	ipThis="${ipThis}_$(ifconfig eth0 | grep 'inet addr' | cut -d':' -f2 | cut -d' ' -f1)"
#fi
DeployDir=$MountDir/`date  --iso-8601`_$ipThis
mkdir $DeployDir
mkdir $DeployDir/contexts
mkdir $DeployDir/logs

if [[ "$coreCount" != "0" ]] ;then

	## Create and copy context to 150
	for binFile in $binFiles ;do
		if [[ "$(which $binFile)" != "" ]] ;then
			binFile=$(which $binFile)
		else
			echo "warning: bin file is not found ($binFile)"
			binFile=$(echo /usr/pluto/bin/${binFile}* | cut -d' ' -f1)
		fi

		binLibs=$(ldd /usr/pluto/bin/Text_To_Speech  | cut -d ">" -f 2 |  cut -d'(' -f1)
	
		rm -rf /context_`basename $binFile`
		mkdir /context_`basename $binFile`
	
		for file in $binFile $binLibs ; do
			ln -s $file /context_`basename $binFile`/
		done
	
		tar -chf $DeployDir/contexts/context_`basename $binFile`.tar /context_`basename $binFile`
	
		rm -rf /context_`basename $binFile`
	done

	## Copy coredumps to 150
	cp /usr/pluto/coredump/* $DeployDir
fi

## Copy the logs to 150
cp -Lr /var/log/* "${DeployDir}/logs/"

umount $MountDir
rmdir $MountDir

exec 1>&6 2>&6 6>&-
