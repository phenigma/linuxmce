#!/bin/bash

#nobuild="-b"
nobuild=""

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

if [ "x$nobuild" = "x" ]; then
	rm /tmp/main_sqlcvs.dump
    if [ $version -eq 1 ]; then
        O1="UPDATE Version SET VersionName='$(date +%g%m%d%H)' WHERE PK_Version=$version;"
        echo $O1 | mysql pluto_main

		#This is an hourly build, so we're going to dump the pluto_main database and make it our sqlCVS database
		mysqldump --quote-names --allow-keywords --add-drop-table pluto_main > /tmp/main_sqlcvs.dump
	else
		#This is a release build, so we want to get a real sqlCVS 
		rm /tmp/main_sqlcvs.tar.gz
		ssh uploads@plutohome.com "rm /tmp/main_sqlcvs.dump /home/uploads/main_sqlcvs.tar.gz; mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata main_sqlcvs > /tmp/main_sqlcvs.dump; cd /tmp; tar zcvf /home/uploads/main_sqlcvs.tar.gz main_sqlcvs.dump"
		scp uploads@plutohome.com:/home/uploads/main_sqlcvs.tar.gz /tmp/
		cd /tmp
		tar zxvf main_sqlcvs.tar.gz
    fi

	if [ ! -f /tmp/main_sqlcvs.dump ]; then
		echo "sqlcvs.dump not found.  aborting"
		read
		exit
	fi
	
	mysql main_sqlcvs < /tmp/main_sqlcvs.dump
    
	if [ $version -eq 1 ]; then
		sqlCVS -h localhost -D main_sqlcvs update-psc
	fi
	sqlCVS -h localhost -D pluto_security update-psc
	sqlCVS -h localhost -D pluto_media update-psc

    rm -rf /home/MakeRelease
    mkdir -p /home/MakeRelease
    cd /home/MakeRelease
	svn co http://10.0.0.170/pluto/trunk/. | tee /home/MakeRelease/svn.log
	mkdir -p /home/MakeRelease/trunk/src/bin
	cp /home/builds/Windows_Output/src/bin/* /home/MakeRelease/trunk/src/bin
    cd /home/MakeRelease/trunk
    svn info > svn.info
else
    cd /home/MakeRelease/trunk
fi

#Do some database maintenance to correct any errors
# Be sure all debian packages are marked as being compatible with debian distro
MQ1="UPDATE Package_Source_Compat   JOIN Package_Source on FK_Package_Source=PK_Package_Source  SET FK_OperatingSystem=NULL,FK_Distro=1  WHERE FK_RepositorySource=2";
echo $MQ1 | mysql pluto_main

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


if [ -d "$BASE_OUT_FOLDER/$version_name" ]; then
	[ -d "$BASE_OUT_FOLDER/$version_name.auto_backup" ] && rm -rf "$BASE_OUT_FOLDER/$version_name.auto_backup";
	echo It seems like the same build was created before. Backing up the previous version.
	echo Warning !! I will only keep 1 backup of the build. You should save the backup while
	echo this build is running if you needs it earlier. 
	echo     This is the backed up folder: $BASE_OUT_FOLDER/$version_name.auto_backup
	echo 
	mv "$BASE_OUT_FOLDER/$version_name" "$BASE_OUT_FOLDER/$version_name.auto_backup"
fi;

# Creating target folder.
mkdir -p "$BASE_OUT_FOLDER/$version_name";

if ! MakeRelease $nobuild -a -o 1 -r 2,9,11 -m 1 -s /home/MakeRelease/trunk -n / -R $svninfo -v $version > /home/MakeRelease/MakeRelease1.log ; then
	echo "MakeRelease Failed.  Press any key"
	reportError
	read
	exit
fi

`dirname $0`/scripts/propagate.sh "$BASE_OUT_FOLDER/$version_name/"
pushd "$BASE_INSTALLATION_CD_FOLDER"; 
"$BASE_INSTALLATION_CD_FOLDER/go-netinst.pl" "$BASE_OUT_FOLDER/$version_name" cache;
popd
pushd "$BASE_INSTALLATION_2_6_10_CD_FOLDER"; 
"$BASE_INSTALLATION_2_6_10_CD_FOLDER/go-netinst.pl" "$BASE_OUT_FOLDER/$version_name" cache;
popd

echo Setting this version as the current one.
rm $BASE_OUT_FOLDER/current
ln -s $BASE_OUT_FOLDER/$version_name $BASE_OUT_FOLDER/current 

#mv /home/builds/$version_name/debian-packages.tmp /home/builds/$version_name/debian-packages.list

if ! MakeRelease -a -o 7 -n / -s /home/samba/builds/Windows_Output/ -r 10 -v $version -b -k 116,119,124,126,154,159,193,203,213,226,237,242,255,277,204,118,303,128,162,191,195,280,272,362,363 > /home/MakeRelease/MakeRelease2.log ; then
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

read

