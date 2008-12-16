
. /usr/pluto/bin/Config_Ops.sh || :

(
	cd /usr/pluto/database
	/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D pluto_media -r media import
) || exit $?

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

Q="GRANT ALL PRIVILEGES ON pluto_media.* TO 'plutomedia'@'localhost';"
mysql $MYSQL_DB_CRED -e "$Q"

Q="GRANT ALL PRIVILEGES ON pluto_media.* TO '$MySqlUser'@'127.0.0.1' IDENTIFIED BY '$MySqlPassword';"
mysql $MYSQL_DB_CRED -e "$Q"

Q="FLUSH PRIVILEGES;"
mysql $MYSQL_DB_CRED -e "$Q"

#temporary code -- we got rid of the 
Q="UPDATE Attribute SET FK_AttributeType=13 WHERE FK_AttributeType=4;"
mysql $MYSQL_DB_CRED pluto_media -e "$Q"

