(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -n -D pluto_security -r security import) || exit $?

Q="GRANT ALL PRIVILEGES ON pluto_security.* TO 'plutosecurity'@'localhost';"
echo "$Q" | /usr/bin/mysql

Q="GRANT ALL PRIVILEGES ON pluto_security.* TO 'root'@'127.0.0.1'; FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql

#temporary code because the filter wasn't set properly before
echo "delete from Alert where psc_id is not null and psc_id>0;" | mysql pluto_security
echo "delete from ModeChange where psc_id is not null and psc_id>0;" | mysql pluto_security
echo "delete from Notification where psc_id is not null and psc_id>0;" | mysql pluto_security
echo "delete from Picture where psc_id is not null and psc_id>0;" | mysql pluto_security

