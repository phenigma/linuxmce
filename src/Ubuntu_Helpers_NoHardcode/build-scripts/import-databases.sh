#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
#set -x

function import_databases () {
#	local dbdump_main_sqlcvs=$(mktemp)
#	local dbdump_myth_sqlcvs=$(mktemp)
#	local dbdump_pluto_media=$(mktemp)
#	local dbdump_pluto_security=$(mktemp)
#	local dbdump_pluto_telecom=$(mktemp)

#	local dbdump_dir=$(mktemp -d)
	local dbdump_dir="${build_dir}/tmp/databases"
	local dbdump_main_sqlcvs="${dbdump_dir}/main_sqlcvs.sql"
	local dbdump_myth_sqlcvs="${dbdump_dir}/myth_sqlcvs.sql"
	local dbdump_pluto_media="${dbdump_dir}/pluto_media.sql"
	local dbdump_pluto_security="${dbdump_dir}/pluto_security.sql"
	local dbdump_pluto_telecom="${dbdump_dir}/telecom.sql"

	mkdir -p "${dbdump_dir}"


	## Import pluto_main, pluto_media, pluto_security, pluto_telecom
	DisplayMessage "Getting a sqldump of main_sqlcvs"
	[ ! -e "$dbdump_main_sqlcvs" ] && #ZaercTempHack
	mysqldump --compress -u "$sqlcvs_user" -h "$sqlcvs_host" main_sqlcvs    > "$dbdump_main_sqlcvs"
	DisplayMessage "Getting a sqldump of myth_sqlcvs"
	[ ! -e "$dbdump_myth_sqlcvs" ] && #ZaercTempHack
	mysqldump --compress -u "$sqlcvs_user" -h "$sqlcvs_host" myth_sqlcvs    > "$dbdump_myth_sqlcvs"
	DisplayMessage "Getting a sqldump of pluto_media"
	[ ! -e "$dbdump_pluto_media" ] && #ZaercTempHack
	mysqldump --compress -u "$sqlcvs_user" -h "$sqlcvs_host" pluto_media    > "$dbdump_pluto_media"
	DisplayMessage "Getting a sqldump of pluto_security"
	[ ! -e "$dbdump_pluto_security" ] && #ZaercTempHack
	mysqldump --compress -u "$sqlcvs_user" -h "$sqlcvs_host" pluto_security > "$dbdump_pluto_security"
	DisplayMessage "Getting a sqldump of pluto_telecom"
	[ ! -e "$dbdump_pluto_telecom" ] && #ZaercTempHack
	mysqldump --compress -u "$sqlcvs_user" -h "$sqlcvs_host" pluto_telecom  > "$dbdump_pluto_telecom"

	# Run search and replace over the db dumps
	DisplayMessage "Running Search'n'Replace on the sqldumps (MakeRelase_PrepFiles)"
	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/${svn_branch_name}/src/lib"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	$MakeRelease_PrepFiles -p $(dirname $dbdump_main_sqlcvs) -e "$(basename $dbdump_main_sqlcvs),$(basename $dbdump_myth_sqlcvs),$(basename $dbdump_pluto_media),$(basename $dbdump_pluto_security),$(basename $dbdump_pluto_telecom)" -c /etc/lmce-build/${flavor}.conf

	
	# pluto_main_build & pluto_main
	DisplayMessage "Importing pluto_main database"
	echo "DROP DATABASE IF EXISTS pluto_main_build;"| mysql -h ${sql_build_host} -u ${sql_build_user}
	echo "CREATE DATABASE pluto_main_build;"	| mysql -h ${sql_build_host} -u ${sql_build_user}
	cat "$dbdump_main_sqlcvs"			| mysql -h ${sql_build_host} -u ${sql_build_user} "pluto_main_build"
	echo "DROP DATABASE IF EXISTS pluto_main;"	| mysql -h ${sql_build_host} -u ${sql_build_user}
	echo "CREATE DATABASE pluto_main;"		| mysql -h ${sql_build_host} -u ${sql_build_user}
	cat "$dbdump_main_sqlcvs"			| mysql -h ${sql_build_host} -u ${sql_build_user} "pluto_main"

	# main_sqlcvs_ubuntu
	DisplayMessage "Importing main_sqlcvs database"
	echo "DROP DATABASE IF EXISTS main_sqlcvs_${flavor};"	| mysql -h ${sql_build_host} -u ${sql_build_user}
	echo "CREATE DATABASE main_sqlcvs_${flavor};"	| mysql -h ${sql_build_host} -u ${sql_build_user}
	cat "$dbdump_main_sqlcvs"			| mysql -h ${sql_build_host} -u ${sql_build_user} "main_sqlcvs_${flavor}"
	echo 'DELETE FROM `Package_Version`;'		| mysql -h ${sql_build_host} -u ${sql_build_user} "main_sqlcvs_${flavor}"
	echo 'DELETE FROM `Schema`;'			| mysql -h ${sql_build_host} -u ${sql_build_user} "main_sqlcvs_${flavor}"

	# myth_sqlcvs_ubuntu
	DisplayMessage "Importing myth_sqlcvs database"
	echo "DROP DATABASE IF EXISTS myth_sqlcvs_${flavor};"	| mysql -h ${sql_build_host} -u ${sql_build_user}
	echo "CREATE DATABASE myth_sqlcvs_${flavor};"	| mysql -h ${sql_build_host} -u ${sql_build_user}
	cat "$dbdump_myth_sqlcvs"			| mysql -h ${sql_build_host} -u ${sql_build_user} "myth_sqlcvs_${flavor}"

	# pluto_media
	DisplayMessage "Importing pluto_media database"
	echo "DROP DATABASE IF EXISTS pluto_media;"	| mysql -h ${sql_build_host} -u ${sql_build_user}
	echo "CREATE DATABASE pluto_media;"		| mysql -h ${sql_build_host} -u ${sql_build_user}
	cat "$dbdump_pluto_media"			| mysql -h ${sql_build_host} -u ${sql_build_user} "pluto_media"

	# pluto_security
	DisplayMessage "Importing pluto_security database"
	echo "DROP DATABASE IF EXISTS pluto_security;"	| mysql -h ${sql_build_host} -u ${sql_build_user}
	echo "CREATE DATABASE pluto_security;"		| mysql -h ${sql_build_host} -u ${sql_build_user} 
	cat "$dbdump_pluto_security" 			| mysql -h ${sql_build_host} -u ${sql_build_user} "pluto_security"
	
	# pluto_telecom
	DisplayMessage "Importing pluto_telecom database"
	echo "DROP DATABASE IF EXISTS pluto_telecom;"	| mysql -h ${sql_build_host} -u ${sql_build_user}
	echo "CREATE DATABASE pluto_telecom;"		| mysql -h ${sql_build_host} -u ${sql_build_user}
	cat "$dbdump_pluto_telecom"			| mysql -h ${sql_build_host} -u ${sql_build_user} "pluto_telecom"

#	rm -f "$dbdump_main_sqlcvs" "$dbdump_myth_sqlcvs" "$dbdump_pluto_media" "$dbdump_pluto_security" "$dbdump_pluto_telecom"
}

DisplayMessage "*** STEP: Importing Databases"
trap 'Error "Undefined error in $0"' EXIT

import_databases

trap - EXIT
