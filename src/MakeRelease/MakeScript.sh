#!/bin/bash

#nobuild="-b"
nobuild=""

# If not version was specified then we will assume the latest specified version from the database 
if [ "$1" = "" ]; then
	# lock the svn commits only to the authorized personnel
 
	if [ "x$nobuild" = "x" ]; then
		cp -f /var/lib/svn-repos/pluto2/conf/users-restricted /var/lib/svn-repos/pluto2/conf/users
    fi
 	Q="select PK_Version from Version WHERE PK_Version<>1 ORDER BY date desc, PK_Version limit 1"
	version=$(echo "$Q;" | mysql -N pluto_main)
else
	cp -f /var/lib/svn-repos/pluto2/conf/users-all /var/lib/svn-repos/pluto2/conf/users
	version=$1
fi

echo Using version with id: "$version"

rm /home/tmp/pluto-build/*.deb
rm -rf /home/tmp/pluto-build/pluto-*
rm -rf /var/www/download/deb_sarge /var/www/download/source /var/www/download/win2000xp /var/www/download/wince /var/www/download/symbian_60

if [ "x$nobuild" = "x" ]; then
    if [ $version -eq 1 ]; then
        O1="UPDATE Version SET VersionName='$(date +%g%m%d%H)' WHERE PK_Version=$version;"
        echo $O1 | mysql pluto_main
    fi

    rm -rf /home/MakeRelease
    mkdir -p /home/MakeRelease
    cd /home/MakeRelease
	svn co http://10.0.0.170/pluto/trunk/. | tee /home/MakeRelease/svn.log
    cd /home/MakeRelease/trunk
    svn info > svn.info
else
    cd /home/MakeRelease/trunk
fi

svninfo=$(svn info . |grep ^Revision | cut -d" " -f2)
O2="UPDATE Version SET SvnRevision=$svninfo WHERE PK_Version=$version;"
echo $O2 | mysql pluto_main
echo $O2 > /home/MakeRelease/query2

Q3="select VersionName from Version WHERE PK_Version=$version"
version_name=$(echo "$Q3;" | mysql -N pluto_main)

DEST="mihai.t@newflavorstudio.com -c igor.s@newflavorstudio.com -c aaron@plutohome.com -c radu.c@newflavorstudio.com -c mtoader@gmail.com"

function reportError
{
    echo "MakeRelease failed.";

    [ -e "/home/MakeRelease/MakeRelease.log" ] && (echo -e "Make Release failed. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 /home/MakeRelease/MakeRelease.log) | mail -s "MakeRelease failes" $DEST
#   [ -e "$3/svn-checkout.log" ] && (echo -e "Build failed $1. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $3/svn-checkout.log) | mail -s "Build failure for revision $2" $DEST

}

if ! MakeRelease $nobuild -a -o 1 -r 2,9,11 -m 1 -s /home/MakeRelease/trunk -n / -R $svninfo -v $version > /home/MakeRelease/MakeRelease1.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

mv /home/tmp/pluto-build/debian-packages.tmp /home/tmp/pluto-build/debian-packages.list

if ! MakeRelease -a -o 7 -n / -s /var/www/WinMakeRelease_output/ -r 10 -v $version -b -k 116,119,124,126,154,159,193,203,213,226,237,242,255,277,290,204,118,303 > /home/MakeRelease/MakeRelease2.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -a -o 7 -n / -s /home/MakeRelease/trunk -r 10 -v $version -b -k 211,214,233,256,219,220 > /home/MakeRelease/MakeRelease3.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -a -o 12 -n / -s /var/www/WinMakeRelease_output/ -r 15 -v $version -b -k 119 > /home/MakeRelease/MakeRelease4.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -a -o 8 -n / -s /var/www/WinMakeRelease_output/ -r 16 -v $version -b -k 119 > /home/MakeRelease/MakeRelease5.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

	cd /home/tmp/pluto-build/
	./propagate.sh
	(cd /home/Pluto-D-i; ./go-netinst cache $version_name; cp DSP2.iso /var/www/download/cds/KickStart_$version_name.iso)
	if [ $version -ne 1 ]; then
		# SourceForge CVS
		if ! MakeRelease -a -o 1 -r 12 -m 1 -s /home/MakeRelease/trunk -n / -b -v $version  > /home/MakeRelease/MakeRelease6.log ; then
			reportError
			echo "MakeRelease to source forge CVS Failed.  Press any key"
		    read
		fi

		# SourceForge Debian Sarge
		if ! MakeRelease -a -o 1 -r 13 -m 1 -s /home/MakeRelease/trunk -n / -b -v $version  > /home/MakeRelease/MakeRelease7.log ; then
			reportError
			echo "MakeRelease to source forge CVS Failed.  Press any key"
		    read
		fi

		# SourceForge Windows Archives
		if ! MakeRelease -a -o 1 -r 17 -m 1 -s /home/MakeRelease/trunk -n / -b -v $version  > /home/MakeRelease/MakeRelease8.log ; then
			reportError
			echo "MakeRelease to source forge CVS Failed.  Press any key"
		    read
		fi

		# SourceForge Source Archives
		if ! MakeRelease -a -o 1 -r 18 -m 1 -s /home/MakeRelease/trunk -n / -b -v $version  > /home/MakeRelease/MakeRelease9.log ; then
			reportError
			echo "MakeRelease to source forge CVS Failed.  Press any key"
		    read
		fi
		
		(echo -e "Need to propagate new SourceForge\n\n") | mail -s "SourceForge" dan.h@plutohome.com -c aaron@plutohome.com
		
		cd /home
	
		rm debian.tar.gz
		tar zcvf /home/debian.tar.gz /var/www/debian/dists/20dev/main/binary-i386/*
		scp debian.tar.gz problems@69.25.176.44:~/

		rm download.tar.gz
		tar zcvf /home/download.tar.gz /var/www/download/*
		scp download.tar.gz problems@69.25.176.44:~/

		sh -x /var/www/DumpVersionPackage.sh
		scp dumpvp.tar.gz problems@69.25.176.44:~/

		echo "Sent to server."
	fi
	echo "Everything okay.  Press any key"

	if [ "x$nobuild" = "x" ]; then
		(echo -e "MakeRelease $version ok\n\n") | mail -s "MakeRelease $version ok" mihai.t@plutohome.com -c aaron@plutohome.com
	else
		(echo -e "MakeRelease $version ok\n\nNeed to reset nobuild flag") | mail -s "**reset nobuild flag** MakeRelease $version ok" mihai.t@plutohome.com -c aaron@plutohome.com
	fi
read

