#!/bin/bash

# Mantis 2692: 
#
# The table InfraredGroup_Command in .39 contain all IR codes, and it wasn't updated on .40 upgrade.
# That make that codes with FK_InfraredGroup=NULL to remain in database, although the logic in .40 was 
# changed, and such codes are no longer posible.
# Having FK_InfraredGroup null prevent codes to be displayed on web pages, and since the table wasn't 
# updated, even running sqlCVS for them won't update them (they have psc_mod=0).

MyName="002-000-040-IncorrectRecordsInInfraredGroup_Command.sh"

. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null || exit 0

Q="SELECT psc_id FROM InfraredGroup_Command WHERE FK_InfraredGroup IS NULL"
R=$(RunSQL "$Q")

psc_ids=""
for psc_id in $R; do
	if [[ "$psc_ids" != "" ]] ;then
		psc_ids="${psc_ids},${psc_id}"
	else
		psc_id="${psc_id}"
	fi
done

## If the records are not updated
if [[ "$psc_ids" != "" ]] ;then
	## Try an update right now
	/usr/pluto/bin/WebDB_Get.sh "http://plutohome.com/GetInfraredCodes.php?psc_ids=${psc_ids}" 'InfraredGroup_Command'

	## Make sure that we are runned in cron hourly, too :)
	if [[ ! -f /etc/cron.hourly/$MyName ]] ;then
		cp /usr/share/pluto-upgrade-helper/$MyName /etc/cron.hourly/
	fi
else
	## My job here is done, I'll be back .. 
	rm -rf /etc/cron.hourly/$MyName
fi
