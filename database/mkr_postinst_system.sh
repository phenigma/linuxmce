if ! echo 'SELECT PK_DeviceTemplate FROM DeviceTemplate limit 1;' | mysql pluto_main; then
	(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -D pluto_main -r constants,dce,designer,document,ir,website import)
else
    #if the table was already modified, this won't hurt anything
    echo 'System database already installed'
    # Don't want an error code
    date
fi

