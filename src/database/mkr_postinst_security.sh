
. /usr/pluto/bin/Config_Ops.sh || :

(
	cd /usr/pluto/database
	/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D pluto_security -r security import
) || exit $?

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

Q="GRANT ALL PRIVILEGES ON pluto_security.* TO 'plutosecurity'@'localhost';"
mysql $MYSQL_DB_CRED -e "$Q"

Q="GRANT ALL PRIVILEGES ON pluto_security.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
mysql $MYSQL_DB_CRED -e "$Q"

Q="FLUSH PRIVILEGES;"
mysql $MYSQL_DB_CRED -e "$Q"

