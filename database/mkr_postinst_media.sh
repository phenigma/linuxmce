(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -D pluto_media -r media import) || exit $?

Q="GRANT ALL PRIVILEGES ON pluto_media.* TO 'plutomedia'@'localhost'; FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql
