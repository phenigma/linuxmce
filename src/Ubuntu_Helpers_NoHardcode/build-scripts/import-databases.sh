#!/bin/bash

. /etc/lmce-build/builder.conf

set -e
set -x

function import_databases () {
	local dbdump_pluto_main=$(mktemp)
	local dbdump_main_sqlcvs=$(mktemp)
	local dbdump_myth_sqlcvs=$(mktemp)
	local dbdump_pluto_media=$(mktemp)
	local dbdump_pluto_security=$(mktemp)
	local dbdump_pluto_telecom=$(mktemp)

  #TODO START: Replace this when new secure way of getting the databases it available
	## Import sqlcvs repositories from plutohome.com
	local temp_sqlcvsdir=$(mktemp -d)
	ssh -i /etc/lmce-build/builder.key uploads@plutohome.com "
                set -x;
                rm -f /tmp/main_sqlcvs.dump /tmp/myth_sqlcvs /home/uploads/sqlcvs_dumps.tar.gz;
                mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata main_sqlcvs > /tmp/main_sqlcvs.dump;
                mysqldump --quote-names --allow-keywords --add-drop-table -u root -pmoscow70bogata myth_sqlcvs > /tmp/myth_sqlcvs.dump;
                cd /tmp;
                tar zcvf /home/uploads/sqlcvs_dumps.tar.gz main_sqlcvs.dump myth_sqlcvs.dump"
        scp -i /etc/lmce-build/builder.key uploads@plutohome.com:/home/uploads/sqlcvs_dumps.tar.gz "$temp_sqlcvsdir"
	pushd "$temp_sqlcvsdir"
	        tar zxvf sqlcvs_dumps.tar.gz
		mv main_sqlcvs.dump $dbdump_main_sqlcvs
		mv myth_sqlcvs.dump $dbdump_myth_sqlcvs 
	popd
	
	## Import other databases from 150
	ssh -i /etc/lmce-build/builder.key pluto@82.77.255.209 "
		set -x;
		mysqldump -u root pluto_main     > /tmp/pluto_main.dump;
		mysqldump -u root pluto_media    > /tmp/pluto_media.dump;
		mysqldump -u root pluto_security > /tmp/pluto_security.dump;
		mysqldump -u root pluto_telecom  > /tmp/pluto_telecom.dump;
		cd /tmp;
		tar zcvf /tmp/sqldumps.tar.gz *.dump
	"
	scp -i /etc/lmce-build/builder.key pluto@82.77.255.209:/tmp/sqldumps.tar.gz "$temp_sqlcvsdir"
	pushd "$temp_sqlcvsdir"
		tar zxvf sqldumps.tar.gz
		mv pluto_main.dump $dbdump_pluto_main
		mv pluto_media.dump $dbdump_pluto_media
		mv pluto_security.dump $dbdump_pluto_security
		mv pluto_telecom.dump $dbdump_pluto_telecom
	popd
	rm -rf "$temp_sqlcvsdir"
  #TODO END: Replace this when new secure way of getting the databases it available

	# Run search and replace over the db dumps
	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	$MakeRelease_PrepFiles -p $(dirname $dbdump_pluto_main) -e "$(basename $dbdump_pluto_main),$(basename $dbdump_main_sqlcvs),$(basename $dbdump_myth_sqlcvs),$(basename $dbdump_pluto_media),$(basename $dbdump_pluto_security),$(basename $dbdump_pluto_telecom)" -c ${build_dir}/${flavor}.conf

	
	# pluto_main_build & pluto_main
	echo "DROP DATABASE pluto_main_build;"		| mysql -h localhost -u root "pluto_main_build"
	echo "CREATE DATABASE pluto_main_build;"	| mysql -h localhost -u root "pluto_main_build"
	cat "$dbdump_pluto_main"			| mysql -h localhost -u root "pluto_main_build"
	echo "DROP DATABASE pluto_main;"		| mysql -h localhost -u root "pluto_main"
	echo "CREATE DATABASE pluto_main;"		| mysql -h localhost -u root "pluto_main"
	cat "$dbdump_pluto_main"			| mysql -h localhost -u root "pluto_main"
	rm -f "$dbdump_pluto_main"

	# main_sqlcvs_ubuntu
	echo "DROP DATABASE main_sqlcvs_${flavor};"	| mysql -h localhost -u root "main_sqlcvs_${flavor}"
	echo "CREATE DATABASE main_sqlcvs_${flavor};"	| mysql -h localhost -u root "main_sqlcvs_${flavor}"
	cat "$dbdump_main_sqlcvs"			| mysql -h localhost -u root "main_sqlcvs_${flavor}"
	echo 'DELETE FROM `Package_Version`;'		| mysql -h localhost -u root "main_sqlcvs_${flavor}"
	echo 'DELETE FROM `Schema`;'			| mysql -h localhost -u root "main_sqlcvs_${flavor}"
	rm -f "$dbdump_main_sqlcvs"

	# myth_sqlcvs_ubuntu
	echo "DROP DATABASE myth_sqlcvs_${flavor};"	| mysql -h localhost -u root "myth_sqlcvs_${flavor}" 
	echo "CREATE DATABASE myth_sqlcvs_${flavor};"	| mysql -h localhost -u root "myth_sqlcvs_${flavor}"
	cat "$dbdump_myth_sqlcvs"			| mysql -h localhost -u root "myth_sqlcvs_${flavor}"
	rm -f "$dbdump_myth_sqlcvs"

	# pluto_media
	echo "DROP DATABASE pluto_media;"		| mysql -h localhost -u root "pluto_media"
	echo "CREATE DATABASE pluto_media;"		| mysql -h localhost -u root "pluto_media"
	cat "$dbdump_pluto_media"			| mysql -h localhost -u root "pluto_media"
	rm -f "$dbdump_pluto_media"

	# pluto_security
	echo "DROP DATABASE pluto_security;"		| mysql -h localhost -u root "pluto_security"
	echo "CREATE DATABASE pluto_security;"		| mysql -h localhost -u root "pluto_security"
	cat "$dbdump_pluto_security" 			| mysql -h localhost -u root "pluto_security"
	rm -f "$dbdump_pluto_security"
	
	# pluto_telecom
	echo "DROP DATABASE pluto_telecom;"		| mysql -h localhost -u root "pluto_telecom"
	echo "CREATE DATABASE pluto_telecom;"		| mysql -h localhost -u root "pluto_telecom"
	cat "$dbdump_pluto_telecom"			| mysql -h localhost -u root "pluto_telecom"
	rm -f "$dbdump_pluto_telecom"
}

import_databases
