(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -n -D pluto_myth -r myth import) || exit $?

Q="GRANT ALL PRIVILEGES ON pluto_myth.* TO 'root'@'localhost';"
echo "$Q" | /usr/bin/mysql

Q="GRANT ALL PRIVILEGES ON pluto_myth.* TO 'root'@'127.0.0.1'; FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql

