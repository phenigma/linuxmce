#!/bin/bash
	rm /tmp/main_sqlcvs.dump
	#This is a release build, so we want to get a real sqlCVS
	bash -x /home/database-dumps/sync-sqlcvs.sh
	rm /tmp/main_sqlcvs.tar.gz
	ssh uploads@plutohome.com "rm /tmp/main_sqlcvs.dump /home/uploads/main_sqlcvs.tar.gz; mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata main_sqlcvs > /tmp/main_sqlcvs.dump; cd /tmp; tar zcvf /home/uploads/main_sqlcvs.tar.gz main_sqlcvs.dump"
	scp uploads@plutohome.com:/home/uploads/main_sqlcvs.tar.gz /tmp/
	cd /tmp
	tar zxvf main_sqlcvs.tar.gz

	if [ ! -f /tmp/main_sqlcvs.dump ]; then
		echo "sqlcvs.dump not found.  aborting"
		read
		exit
	fi

	mysql main_sqlcvs < /tmp/main_sqlcvs.dump
    
