#!/bin/bash

cd /usr/pluto/database/
/usr/pluto/bin/sqlCVS -D pluto_main -r local import

# The following open up ports 22 and 80 and 3450 (DCERouter)  on the Firewall.  This is for development mode only.  The release versions will have the firewall closed by default
echo "insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '22',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );" | mysql pluto_main
echo "insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '80',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );" | mysql pluto_main
echo "insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '3450',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );" | mysql pluto_main


