echo 
echo 
echo Setting up system database.
echo ****This may take a long time****
echo This involves merging all the changes from the
echo central database back into your local copy.
echo Please be patient.......
(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -D pluto_main -r constants,dce,designer,document,ir,website import) || exit $?

