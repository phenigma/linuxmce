(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -n -D pluto_media -r media import) || exit $?

Q="GRANT ALL PRIVILEGES ON pluto_media.* TO 'plutomedia'@'localhost';"
echo "$Q" | /usr/bin/mysql

Q="GRANT ALL PRIVILEGES ON pluto_media.* TO 'root'@'127.0.0.1'; FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql

#temporary code -- accidentally all the media from our test system was distributed
echo "delete from Attribute where psc_id is not null and psc_id>0;" | mysql pluto_media
echo "delete from File where psc_id is not null and psc_id>0;" | mysql pluto_media
echo "delete from File_Attribute where psc_id is not null and psc_id>0;" | mysql pluto_media
echo "delete from Picture where psc_id is not null and psc_id>0;" | mysql pluto_media
echo "delete from Picture_Attribute where psc_id is not null and psc_id>0;" | mysql pluto_media
echo "delete from Picture_File where psc_id is not null and psc_id>0;" | mysql pluto_media
echo "delete from SearchToken where psc_id is not null and psc_id>0;" | mysql pluto_media
echo "delete from SearchToken_Attribute where psc_id is not null and psc_id>0;" | mysql pluto_media
echo "delete from Playlist where psc_id is not null and psc_id>0;" | mysql pluto_media
echo "delete from PlaylistEntry where psc_id is not null and psc_id>0;" | mysql pluto_media

