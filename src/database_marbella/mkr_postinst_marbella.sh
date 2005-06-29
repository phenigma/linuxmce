echo 
echo 
echo Setting up marbella database.
echo ****This may take a long time****
echo This involves merging all the changes from the
echo central database back into your local copy.
echo Please be patient.......
(cd /usr/pluto/database_marbella; /usr/pluto/bin/sqlCVS -n -D pluto_main -r dce,designer import) || exit $?

