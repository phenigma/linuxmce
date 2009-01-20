
. /usr/pluto/bin/Config_Ops.sh || :

(
	cd /usr/pluto/database
	/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D lmce_game -r game import
) || exit $?

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

Q="GRANT ALL PRIVILEGES ON lmce_game.* TO 'plutomedia'@'localhost';"
mysql $MYSQL_DB_CRED -e "$Q"

Q="GRANT ALL PRIVILEGES ON lmce_game.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
mysql $MYSQL_DB_CRED -e "$Q"

Q="FLUSH PRIVILEGES;"
mysql $MYSQL_DB_CRED -e "$Q"

