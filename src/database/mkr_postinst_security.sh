(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -n -D pluto_security -r security import) || exit $?
/usr/pluto/bin/Debug_LogKernelModules.sh "$0"

Q="GRANT ALL PRIVILEGES ON pluto_security.* TO 'plutosecurity'@'localhost';"
echo "$Q" | /usr/bin/mysql

Q="GRANT ALL PRIVILEGES ON pluto_security.* TO 'root'@'127.0.0.1'; FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql

