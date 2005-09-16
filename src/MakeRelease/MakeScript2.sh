#!/bin/bash

#nobuild="-b"
nobuild=""

fastrun=""
#fastrun="-f -DERROR_LOGGING_ONLY"

BASE_OUT_FOLDER=/home/builds/

BASE_INSTALLATION_CD_FOLDER=/home/installation-cd/
BASE_INSTALLATION_2_6_10_CD_FOLDER=/home/installation-cd-kernel-2.6.10/

#
# Uncomment the following function name to allow running of the MakeScript without actually calling 
# the MakeRelease program/
#function MakeRelease { true; }


echo "--" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
echo "--" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
echo "Called at time `date`" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
echo "--" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
ps auxfww >> $BASE_OUT_FOLDER/MakeReleaseCalls.log
echo "--" >> $BASE_OUT_FOLDER/MakeReleaseCalls.log

function changeSvnPermissions
{
	newPerms=$1;

	if [ "x$1" != "" ]; then
		return;
	fi;

	sudo -u www-date sed "s/@restricted = .*/@restricted = $newPerms/" /home/sources/svn-repositories/svn-users-permissions > /home/sources/svn-repositories/svn-users-permissions-updated
	sudo -u www-data cp /home/sources/svn-repositories/svn-users-permissions /home/sources/svn-repositories/svn-users-permissions-bak
	sudo -u www-data cp /home/sources/svn-repositories/svn-users-permissions-updated /home/sources/svn-repositories/svn-users-permissions
}

# If not version was specified then we will assume the latest specified version from the database 
if [ "$1" = "" ]; then
	# lock the svn commits only to the authorized personnel
	if [ "x$nobuild" = "x" ]; then
		changeSvnPermissions r;
    	fi
 	Q="select PK_Version from Version WHERE PK_Version<>1 ORDER BY date desc, PK_Version limit 1"
	version=$(echo "$Q;" | mysql -N pluto_main)
else
	changeSvnPermissions rw;
	version=$1
fi

echo Using version with id: "$version"

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

echo Building version $version_name 


if ! MakeRelease -a -o 7 -n / -s /home/samba/builds/Windows_Output/ -r 10 -v $version -b -k 116,119,124,126,154,159,193,203,213,226,237,242,255,277,204,118,303,128,162,191,195,280,272,363,364,341 > /home/MakeRelease/MakeRelease2.log ; then
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

if ! MakeRelease -a -o 12 -n / -s /home/samba/builds/Windows_Output/ -r 15 -v $version -b -k 119 > /home/MakeRelease/MakeRelease4.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

if ! MakeRelease -a -o 8 -n / -s /home/samba/builds/Windows_Output/ -r 16 -v $version -b -k 119 > /home/MakeRelease/MakeRelease5.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

cp -r /home/samba/builds/Windows_Output/winlib $BASE_OUT_FOLDER/$version_name
cp -r /home/samba/builds/Windows_Output/winnetdlls $BASE_OUT_FOLDER/$version_name
#Moved up ..
#dcd /home/tmp/pluto-build/
#./propagate.sh


if [ $version -ne 1 ]; then
    mkdir -p /home/builds/upload
    pushd /home/builds
    rm upload/download.tar.gz
	cd $version_name
    tar zcvf ../upload/download.tar.gz *
    scp ../upload/download.tar.gz uploads@66.235.209.27:~/

    cd ../upload
    sh -x `dirname $0`/scripts/DumpVersionPackage.sh
    scp dumpvp.tar.gz uploads@66.235.209.27:~/
    popd

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
	
	echo "Sent to server."
fi

echo "Everything okay.  Press any key"

if [ "x$nobuild" = "x" ]; then
	(echo -e "MakeRelease $version ok\n\n") | mail -s "MakeRelease $version ok" mihai.t@plutohome.com -c aaron@plutohome.com
else
	(echo -e "MakeRelease $version ok\n\nNeed to reset nobuild flag") | mail -s "**reset nobuild flag** MakeRelease $version ok" mihai.t@plutohome.com -c aaron@plutohome.com
fi

if [ $version -ne 1 ]; then
	(echo -e "Change version back to 1\n\n") | mail -s "**change version back to 1**" mihai.t@plutohome.com -c aaron@plutohome.com
fi

if [ "x$fastrun" = "x" ]; then
	echo "Normal debug build"
else
	(echo -e "Remove fastrun flag\n\n") | mail -s "**remove fastrun flag**" mihai.t@plutohome.com -c aaron@plutohome.com
fi
sh -x /home/SendToSwiss.sh
read


