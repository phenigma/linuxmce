#!/bin/bash

# Suspended for the moment

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
	error=0
	local URL="$1"
	local Table="$2"

	local OutputFile=$(mktemp)
	wget --timeout=10 -O "$OutputFile" "$URL" 1>/dev/null 2>/dev/null

	local Header=$(head -1 "$OutputFile")
	local Footer=$(tail -1 "$OutputFile")

	if [[ "$Header" != "-- Database import" || "$Footer" != "-- EOF" ]]; then
		echo "* Can't get an correct sql script from the server."
		error=1
		return
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
	echo "* The server updated : $psc_ids_used"
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
	echo "* This remain to be manualy updated: $psc_ids"

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


	# This function modifies $psc_ids (removes the updated ones) and $error (to 1 in case of error)

	if [[ $psc_ids != "" ]] ;then
		echo "* Found folowing psc_ids: $psc_ids"
		error=0
		WebDB_Get "http://plutohome.com/GetInfraredCodes.php?psc_ids=${psc_ids}" 'InfraredGroup_Command'
	else
		echo "* No psc_ids to update"
		error="1"
	fi
	
	if [[ "$error" == "0" ]] ;then
		
		reseted_codes_log=""

		for psc_id in $( echo $psc_ids | tr ',' ' ') ;do
			Q="
				SELECT
					FK_DeviceTemplate,
					DeviceTemplate.FK_InfraredGroup,
					DeviceTemplate.FK_Manufacturer,
					DeviceTemplate.FK_DeviceCategory,
					FK_CommMethod,
					InfraredGroup_Command.psc_batch
				FROM
					InfraredGroup_Command
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
				WHERE
					InfraredGroup_Command.psc_id = '${psc_id}'
				LIMIT 1
			"
			Record=$(RunSQL "$Q")

			DT=$(Field "1" "$Record")
			IRG=$(Field "2" "$Record")
			M=$(Field "3" "$Record")
			DC=$(Field "4" "$Record")
			CM=$(Field "5" "$Record")
			psc_batch=$(Field 6 "$Record")

			if [[ $IRG != "NULL" ]] ;then
				echo "* $psc_id : Is assigned to existing infrared group $IRG"
				Q="UPDATE InfraredGroup_Command SET FK_InfraredGroup = $IRG WHERE InfraredGroup_Command.psc_id = $psc_id"
				RunSQL "$Q"
			else
				Q="INSERT INTO InfraredGroup (Description,FK_Manufacturer,FK_DeviceCategory,FK_CommMethod) VALUES ('Custom codes',$M,$DC,$CM); SELECT LAST_INSERT_ID()"
				InsertID=$(RunSQL "$Q")

				Q="UPDATE InfraredGroup_Command SET FK_InfraredGroup = $InsertID WHERE FK_DeviceTemplate = $psc_id"
				RunSQL "$Q"
				echo "* $psc_id : Is assigned to newly created infrared group $InsertID"
			fi

			reseted_codes_log="${reseted_codes_log}InfraredGroup_Command,${psc_id},${psc_batch}\n"
		done
		
		if [[ "$reseted_codes_log" != "" ]] ;then
			. /usr/pluto/bin/Network_Parameters.sh

			reseted_codes_log="${ExtIP},${PK_Installation},$(date +%s)\n${reseted_codes_log}"
			echo -e $reseted_codes_log

			reseted_codes_log=$(echo $reseted_codes_log | sed -e 's/\\n/%0a/g')
			curl -d reseted_codes_log=${reseted_codes_log} http://www.plutohome.com/log.php
		fi
		psc_ids=""
	fi
	
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
