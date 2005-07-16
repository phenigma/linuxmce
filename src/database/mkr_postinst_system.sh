echo 
echo 
echo Setting up system database.
echo ****This may take a long time****
echo This involves merging all the changes from the
echo central database back into your local copy.
echo Please be patient.......

echo "drop table if exists CriteriaParmNesting_D; drop table if exists CriteriaParm_D; drop table if exists Criteria_D" | mysql pluto_main
(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -n -D pluto_main -r constants,dce,designer,document,ir,website import) || exit $?

Q="GRANT ALL PRIVILEGES ON pluto_main.* TO 'root'@'127.0.0.1'; FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql
