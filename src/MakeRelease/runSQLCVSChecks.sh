#!/bin/bash


DATABASES_TO_CHECK="pluto_main pluto_media pluto_security";


DEST="mihai.t@newflavorstudio.com -c vali.g@plutohome.com -c aaron@plutohome.com -c mtoader@gmail.com"

function reportError
{
	[ -e "$2" ] && (echo -e "Database check failed for database: $1. Attached are the last 50 lines of the relevant log file\n\n"; tail -n 50 $2) | mail -s "Database check failure for $1" $DEST
}

rm -rf $HOME/sqlcvs-logs/
mkdir -p $HOME/sqlcvs-logs/

for i in $DATABASES_TO_CHECK; do 
	echo "Checking database: $i";

	mysqldump -u root --quote-names --allow-keywords --add-drop-table $i > /tmp/db.dump
	echo "DROP DATABASE dbtest" | mysql -u root
	echo "CREATE DATABASE dbtest" | mysql -u root
	mysql -u root dbtest < /tmp/db.dump
	
	if ! sqlCVS -u root -v -D dbtest -n update-psc 1>$HOME/sqlcvs-logs/$i-check.log 2>&1; then
		echo "Database check failed for $i ($?)"
		reportError $i $HOME/sqlcvs-logs/$i-check.log
	fi
done;

exit 0;
