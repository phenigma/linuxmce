#!/bin/bash
Q="select PK_Version from Version ORDER BY date desc,PK_Version limit 1"
version=$(echo "$Q;" | mysql -N pluto_main)

if [ $version -eq 1 ]; then
	O1="UPDATE Version SET VersionName='$(date +%g%m%d%H)' WHERE PK_Version=$version;"
	echo $O1 | mysql pluto_main
fi

rm -rf /home/MakeRelease
rm -rf /var/www/download/deb_sarge /var/www/download/source /var/www/download/win2000xp
mkdir -p /home/MakeRelease
cd /home/MakeRelease
svn co svn://localhost/pluto2/trunk/. | tee /home/MakeRelease/svn.log
cd /home/MakeRelease/trunk
svn info > svn.info
svninfo=$(svn info . |grep ^Revision | cut -d" " -f2)
if [ $version -eq 1 ]; then
    O1="UPDATE Version SET VersionName='$(date +%g%m%d%H)' WHERE PK_Version=$version;"
    echo $O1 | mysql pluto_main
	echo $O1 > /home/MakeRelease/query1
fi
O2="UPDATE Version SET SvnRevision=$svninfo WHERE PK_Version=$version;"
echo $O2 | mysql pluto_main
echo $O2 > /home/MakeRelease/query2
echo MakeRelease -o 1 -r 2,9,11 -m 1 -s /home/MakeRelease/trunk -n / -R $svninfo -v $version > /home/MakeRelease/Command
if ! MakeRelease -o 1 -r 2,9,11 -m 1 -s /home/MakeRelease/trunk -n / -R $svninfo -v $version > /home/MakeRelease/MakeRelease.log ; then
	echo "MakeRelease Failed.  Press any key"
	read
	exit
fi

echo MakeRelease -o 7 -n /var/www/WinMakeRelease_output -s /var/www/WinMakeRelease_output/ -r 10 -v $version -b -k 116,119,124,126,154,159,193,203,213,226,237,242,255,277,290 > /home/MakeRelease/Command2
if ! MakeRelease -o 7 -n /var/www/WinMakeRelease_output -s /var/www/WinMakeRelease_output/ -r 10 -v $version -b -k 116,119,124,126,154,159,193,203,213,226,237,242,255,277,290 > /home/MakeRelease/MakeRelease2.log ; then
	echo "MakeRelease Failed.  Press any key"
	read
	exit
fi

	cd /home/tmp/pluto-build/
	./propagate.sh
	(cd /home/Pluto-D-i; ./go cache; cp DSP.iso /var/www/download/cds/pldebsrg.iso)
	if [ $version -ne 1 ]; then
		if ! MakeRelease -o 1 -r 12 -m 1 -s /home/MakeRelease/trunk -n / -b -v $version > /home/MakeRelease/MakeRelease.log ; then
			echo "MakeRelease to source forge CVS Failed.  Press any key"
		    read
		fi
		cd /home
	
		rm debian.tar.gz
		tar zcvf /home/debian.tar.gz /var/www/debian/dists/20dev/main/binary-i386/*
		scp debian.tar.gz problems@69.25.176.44:~/

		rm download.tar.gz
		tar zcvf /home/download.tar.gz /var/www/download/*
		scp download.tar.gz problems@69.25.176.44:~/

		sh -x /var/www/DumpVersionPackage.sh
		scp dumpvp.tar.gz problems@69.25.176.44:~/
		ech "update Version SET Date='20$(date +%g-%m-%d)' WHERE PK_Version=1" | mysql pluto_main

		echo "Sent to server."
	fi
	echo "Everything okay.  Press any key"
	read
