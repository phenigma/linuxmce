#!/bin/bash

cd /usr/pluto/database/
/usr/pluto/bin/sqlCVS -D pluto_main -r local import

echo "insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '22',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );" | mysql pluto_main
echo "insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '80',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );" | mysql pluto_main


