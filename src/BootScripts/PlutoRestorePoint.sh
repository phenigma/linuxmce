#!/bin/bash
# Pluto System Restore

# vars
MASTERDIR="/home/backup"
BKPDIR=$(date +%Y-%m-%d)
FULLPATH="$MASTERDIR/$BKPDIR"

Usage () {
echo "$0 [command]"
echo "Commands :"
echo "	--backup -> create a system backup"
echo "	--restore -> restore system from a backup file"
exit
}


if [[ -z $1 ]]; then
Usage
fi


# create restore point
if [[ "$1" == "--backup" ]]; then 
	
	rm -rf $MASTERDIR
	if [[ ! -d $MASTERDIR ]]; then
		mkdir -p $MASTERDIR
	fi
	
	if [[ ! -d $BKPDIR ]]; then
        	mkdir -p $MASTERDIR/$BKPDIR
	fi
	# backup mysql 
	mkdir -p $FULLPATH/mysql
	for db in asterisk asteriskcdrdb mythconverg pluto_main pluto_media pluto_myth pluto_security pluto_telecom
	do
		mysqldump -l -u root -B $db > $FULLPATH/mysql/$db-$BKPDIR.sql
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
	cp -r /etc/passwd $FULLPATH/etc
	cp -r /etc/shadow $FULLPATH/etc
	cp -r /etc/apache2 $FULLPATH/etc
	# make archive
	cd $MASTERDIR
	tar cfj backup-$BKPDIR.tar.bz2 $BKPDIR
	echo "Restore point done."
	rm -rf $FULLPATH
fi  

# restore system from backup
if [[ "$1" == "--restore" ]]; then
	cd $MASTERDIR
	tar xfj backup-*.tar.bz2
	BKPDIR=$(find $MASTERDIR -type d -maxdepth 1 -mindepth 1)
	# process data from backup
	cp -r $BKPDIR/etc /etc
	cp -r $BKPDIR/usr /usr
	# restore mysql
	cd $BKPDIR/mysql
	dbs=$(ls)
	dbnames=$(ls | awk -F '-' '{print $1}' )

	# erase current mysql data
	for data1 in $dbnames
	do
		echo "processing database - $data1 -"
		/usr/bin/mysqladmin -f drop $data1 1>/dev/null 2>/dev/null
	done
	# restore old mysql data
	for data2 in $dbs
	do
		echo "Restoring database : $data2 "
		/usr/bin/mysql -u root < $data2
	done
	rm -rf $MASTERDIR
	# restore users
	for i in $(mysql -u root -D pluto_main -N -e "select PK_Users from Users")
        do
                chattr +i /home/user_$i
        done
	for j in $(mysql -u root -D pluto_main -N -e "select UserName from Users")
        do
                chattr +i /home/$j
        done

	find /home -type l | xargs ls -la | xargs rm 1>/dev/null 2>/dev/null
	find /home -name user_* | xargs rm -rf 1>/dev/null 2>/dev/null


	for i in $(mysql -u root -D pluto_main -N -e "select PK_Users from Users")
        do
                chattr -i /home/user_$i
        done
	for j in $(mysql -u root -D pluto_main -N -e "select UserName from Users")
        do
                chattr -i /home/$j
        done
	/bin/bash /usr/pluto/bin/SetupUsers_Homes.sh
	# restore MDs
	for a in $(mysql -u root -D pluto_main -e "select PK_Device from Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL")
		do
			chattr +i /usr/pluto/diskless/$a
		done
	find /usr/pluto/diskless/ -type d -mindepth 1 -maxdepth 1 | xargs rm -rf 1>/dev/null 2>/dev/null 
	
	for a in $(mysql -u root -D pluto_main -e "select PK_Device from Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_DeviceTemplate=28 AND FK_Device_ControlledVia IS NULL")
		do
			chattr -i /usr/pluto/diskless/$a
		done
	
	# reconfigure devices
        /usr/bin/mysql -u root -D pluto_main -e "UPDATE Device SET NeedConfigure=1"
        # install the software again for the devices
        /usr/bin/mysql -u root -D pluto_main -e "DELETE FROM Package_Device"

fi

