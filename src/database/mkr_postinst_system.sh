echo 
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
echo 
echo Setting up system database.
echo ****This may take a long time****
echo This involves merging all the changes from the
echo central database back into your local copy.
echo Please be patient.......

echo "drop table if exists CriteriaParmNesting_D; drop table if exists CriteriaParm_D; drop table if exists Criteria_D" | mysql pluto_main || /bin/true
echo "drop table if exists psc_document_batdet; drop table if exists psc_document_bathdr; drop table if exists psc_document_batuser; drop table if exists psc_document_repset; drop table if exists psc_document_schema; drop table if exists psc_document_tables; drop table if exists Document; drop table if exists Document_Comment;" | mysql pluto_main || /bin/true
echo "alter table \`Document_Comment\` ,change \`Date\` \`Date\` datetime   NOT NULL " | mysql pluto_main || /bin/true

(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -n -D pluto_main -r constants,dce,designer,document,ir,website import) || exit $?
mysql pluto_main < /usr/pluto/database/city.dump

Q="GRANT ALL PRIVILEGES ON pluto_main.* TO 'root'@'127.0.0.1'; FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql

# update quick start icons
/bin/bash /usr/pluto/bin/UpdateQuickStartIcons.sh || /bin/true


