#!/bin/bash
# Pluto System Restore

# vars
MASTERDIR="/home/backup"
BKPDIR=$(date +%Y-%m-%d)
FULLPATH="$MASTERDIR/download/$BKPDIR"

Usage () {
echo "$0 [command]"
echo "Commands :"
echo "	--backup -> create a system backup"
echo "	--restore -> restore system from a backup file"
echo "	--restore --skip-md5 -> system restore without md5 control file"
exit
}


if [[ -z $1 ]]; then
Usage
fi

# create restore point
if [[ "$1" == "--backup" ]]; then 
	
	#rm -rf $MASTERDIR
	if [[ ! -d $MASTERDIR ]]; then
		mkdir -p $MASTERDIR/download
	fi
	
	if [[ ! -d $BKPDIR ]]; then
		mkdir -p $MASTERDIR/download/$BKPDIR
	fi
	# backup mysql 
	mkdir -p $FULLPATH/mysql
	dbtables=$(mysql -u root -D pluto_main -N -e "select Tablename from psc_local_tables")
	dbtables=$(echo $dbtables psc_local_batdet psc_local_bathdr psc_local_batuser psc_local_repset psc_local_schema psc_local_tables)
	
	for table in $dbtables
	do
		/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table -u root pluto_main $table  > $FULLPATH/mysql/$table-$BKPDIR.sql
	done
	# backup pluto-admin
	mkdir -p $FULLPATH/usr/pluto/orbiter
	cp -r /usr/pluto/orbiter/floorplans $FULLPATH/usr/pluto/orbiter
	cp -r /usr/pluto/orbiter/users $FULLPATH/usr/pluto/orbiter
	cp -r /usr/pluto/orbiter/rooms $FULLPATH/usr/pluto/orbiter
	cp -r /usr/pluto/orbiter/scenarios $FULLPATH/usr/pluto/orbiter
	# backup conf files
	mkdir -p $FULLPATH/etc
	cp -r /etc/pluto.conf $FULLPATH/etc
	#cp -r /etc/passwd $FULLPATH/etc
	#cp -r /etc/shadow $FULLPATH/etc
	#cp -r /etc/apache2 $FULLPATH/etc
	# make archive
	cd $MASTERDIR/download	
	tar cfj backup-$BKPDIR.tar.bz2 $BKPDIR
	md5sum backup-$BKPDIR.tar.bz2 > backup-$BKPDIR.md5
	echo "Restore point created."
	rm -rf $FULLPATH
fi  

# restore system from backup
if [[ "$1" == "--restore" ]]; then
	cd $MASTERDIR/upload
	backupfile=$(ls $MASTERDIR/upload | grep tar.bz2)
	md5file=$(ls $MASTERDIR/upload | grep md5)
	if [[ "$2" == "--skip-md5" ]]; then
	
		if [[ "$backupfile" != "" ]]; then
			tar xfj backup-*.tar.bz2
			BKPDIR=$(find $MASTERDIR/upload -type d -maxdepth 1 -mindepth 1)
			# process data from backup
			cp -r $BKPDIR/etc /etc
			cp -r $BKPDIR/usr /usr
			# restore mysql
			cd $BKPDIR/mysql
			dbtables=$(ls)
			# restore old mysql data
			for table in $dbtables
			do
				/usr/bin/mysql -u root -D pluto_main < $table
			done
			rm -rf $MASTERDIR/upload/*
		fi
	else	
		if [[ "$md5file" == "" ]]; then
			echo "Control file not found. Exiting ..."
		fi
		md5test=$(md5sum -c $md5file | awk '{print $2}')
		if [[ "$md5test" != "OK" ]]; then
			echo "File integrity check failed ! Process stopped !"
			exit 1
		else 
			echo ""
                	if [[ "$backupfile" != "" ]]; then
                        	tar xfj backup-*.tar.bz2
                        	BKPDIR=$(find $MASTERDIR/upload -type d -maxdepth 1 -mindepth 1)
                        	# process data from backup
                        	cp -r $BKPDIR/etc /etc
                        	cp -r $BKPDIR/usr /usr
                        	# restore mysql
                        	cd $BKPDIR/mysql
                        	dbtables=$(ls)
                        	# restore old mysql data
                        	for table in $dbtables
                        	do
                                	/usr/bin/mysql -u root -D pluto_main < $table
                        	done
                        	rm -rf $MASTERDIR/upload/*
                	fi

		fi
	fi
fi

