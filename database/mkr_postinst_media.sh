# Don't do an import if the local database is already installed.  We'll test if the Users table exists
if ! echo 'SELECT * FROM Attribute limit 1;' | mysql pluto_media; then
	echo 'Setting up Media Database'
	(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -D pluto_media -r media import)
else
    #if the table was already modified, this won't hurt anything
    echo 'Media database already installed'
	# Don't want an error code
    date
fi
