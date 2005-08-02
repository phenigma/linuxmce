#!/bin/bash

cd /usr/pluto/database/
/usr/pluto/bin/sqlCVS -n -D pluto_main -r local import

# In the .26 release a change was inadvertently made disabling the UpdatePackages for MD's.  Temporarily restore that
echo "update Device_StartupScript set Enabled=1 where FK_StartupScript=34;" | mysql pluto_main

# The following open up ports 22 and 80 and 3450 (DCERouter)  on the Firewall.  This is for development mode only.  The release versions will have the firewall closed by default
set +e
rows="$(echo "select * from Firewall" | mysql -N pluto_main)";
if [[ -z "$rows" ]]; then
	echo "insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '22',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );" | mysql pluto_main
	echo "insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '80',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );" | mysql pluto_main
	echo "insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '3450',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );" | mysql pluto_main
	echo "insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '3306',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );" | mysql pluto_main
fi
set -e
