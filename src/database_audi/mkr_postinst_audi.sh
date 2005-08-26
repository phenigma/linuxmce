echo 
echo 
echo Setting up audi database.
echo ****This may take a long time****
echo This involves merging all the changes from the
echo central database back into your local copy.
echo Please be patient.......
(cd /usr/pluto/database_audi; /usr/pluto/bin/sqlCVS -S 2 -n -D pluto_main -r dce,designer import) || exit $?

