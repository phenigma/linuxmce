#!/bin/bash

# Suspended for the moment
exit 0

# Mantis 2692: 
#
# The table InfraredGroup_Command in .39 contain all IR codes, and it wasn't updated on .40 upgrade.
# That make that codes with FK_InfraredGroup=NULL to remain in database, although the logic in .40 was 
# changed, and such codes are no longer posible.
# Having FK_InfraredGroup null prevent codes to be displayed on web pages, and since the table wasn't 
# updated, even running sqlCVS for them won't update them (they have psc_mod=0).

if [[ -f /etc/diskless.conf ]] ;then
        exit 0
fi


MyName="002-000-040-IncorrectRecordsInInfraredGroup_Command.sh"


function WebDB_Get() {
	local URL="$1"
	local Table="$2"

	local OutputFile=$(mktemp)
	wget --timeout=10 -O "$OutputFile" "$URL"

	local Header=$(head -1 "$OutputFile")
	local Footer=$(tail -1 "$OutputFile")

	if [[ "$Header" != "-- Database import" || "$Footer" != "-- EOF" ]]; then
		echo "ERROR. Header: $Header; Footer: $Footer"
		exit 1
	fi

	local PSC=$(head -2 "$OutputFile" | tail -1)
	local PSC="${PSC#-- psc_id: }"

	if [[ -n "$Table" && -n "$PSC" ]]; then
		local Q="DELETE FROM $Table WHERE psc_id IN ($PSC)"
		RunSQL "$Q"
	fi

	local QPass=
	if [[ -n "$MySqlPassword" ]]; then
		QPass="-p$MySqlPassword"
	fi
	mysql -u $MySqlUser -h $MySqlHost $QPass "$MySqlDBName" <"$OutputFile"

	local psc_ids_used=$(head -3 $OutputFile | grep -m 1 "^-- psc_id:" | cut -d ':' -f2 | tr ',' ' ')
	local psc_ids_old=$(echo $psc_ids | tr ',' ' ')
	psc_ids=""

	# psc_ids = psc_ids_old - psc_ids_used  / isn't bash fun ? X-(
	for psc_id_old in $psc_ids_old ;do

		local isUsed="false"
		for psc_id_used in $psc_ids_used ;do
			if [[ "$psc_id_old" == "$psc_id_used" ]] ;then
				local isUsed="true"
				break
			fi
		done
		
		if [[ "$isUsed" == "false" ]] ;then
	                if [[ "$psc_ids" != "" ]] ;then
        	                psc_ids="${psc_ids},${psc_id_old}"
                	else
                        	psc_ids="${psc_id_old}"
 	               fi	
		fi
	done

	rm -rf $OutputFile
}

psc_ids=""

if [[ -f /usr/pluto/bin/SQL_Ops.sh ]] ;then
	. /usr/pluto/bin/SQL_Ops.sh
	
	Q="SELECT psc_id FROM InfraredGroup_Command WHERE FK_InfraredGroup IS NULL AND psc_mod=0 AND psc_batch > 0"
	R=$(RunSQL "$Q")

	for psc_id in $R; do
		if [[ "$psc_ids" != "" ]] ;then
			psc_ids="${psc_ids},${psc_id}"
		else
			psc_ids="${psc_id}"
		fi
	done
	
	# This function also updates psc_ids (removes the updated ones)
	WebDB_Get "http://plutohome.com/GetInfraredCodes.php?psc_ids=${psc_ids}" 'InfraredGroup_Command'
fi
	


## If the records are not updated
if [[ "$psc_ids" != "" ]] ;then
	## Make sure that we are runned in cron hourly, too :)
	if [[ ! -f /etc/cron.hourly/$MyName ]] ;then
		cp /usr/share/pluto-upgrade-helper/$MyName /etc/cron.hourly/
	fi
else
	## My job here is done, I'll be back .. 
	rm -rf /etc/cron.hourly/$MyName
fi
