(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -n -D pluto_media -r media import) || exit $?

Q="GRANT ALL PRIVILEGES ON pluto_media.* TO 'plutomedia'@'localhost';"
echo "$Q" | /usr/bin/mysql

Q="GRANT ALL PRIVILEGES ON pluto_media.* TO 'root'@'127.0.0.1'; FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql

#temporary code -- we got rid of the 
echo "UPDATE Attribute SET FK_AttributeType=13 WHERE FK_AttributeType=4;" | mysql pluto_media
