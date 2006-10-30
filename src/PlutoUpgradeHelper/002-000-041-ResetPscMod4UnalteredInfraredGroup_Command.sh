#!/bin/bash

# The codes (either IR or GSD) defined by user and with FK_InfraredGroup=NULL are no longer 
# available post .40 upgrade.
#
# For all codes assigned to a device template X and with psc_mod!=0 a new InfarredGroup should 
# be created and all codes assigned to it (the id of InfraredGroup set as FK_InfraredGroup in InfraredGroup_Command table). 
# The id should be set as FK_InfraredGroup in DeviceTemplate table for the PK_DeviceTemplate X.
#
# If the device template X already had a FK_InfraredGroup not null, the new group won't be created, 
# but instead the codes will be assigned to it.


if [[ -f /etc/diskless.conf ]] ;then
        exit 0
fi

MyName="002-000-041-ResetPscMod4UnalteredInfraredGroup_Command.sh"

JobDone="false"

if [[ -f /usr/pluto/bin/SQL_Ops.sh ]] ;then
        . /usr/pluto/bin/SQL_Ops.sh


	Q="
		SELECT 
			psc_id,
			psc_mod 
		FROM 
			InfraredGroup_Command 
		WHERE 
			psc_mod != 0 
		AND 
			psc_id IS NOT NULL
	"
	R=$(RunSQL "$Q")

	url_psc_mods=""
	for Record in $R ;do
		psc_id=$(Field "1" "$Record")
		psc_mod=$(Field "2" "$Record")

		if [[ "$url_psc_mods" == "" ]] ;then
			url_psc_mods="${psc_id},${psc_mod}"
		else
			url_psc_mods="${url_psc_mods},${psc_id},${psc_mod}"
		fi
	done

	## If there are no more psc_mods to fix, then our job is done
	if [[ $url_psc_mods == "" ]] ;then
		JobDone="true"

	## If there are any more to fix, try to fix them now
	else
		OutputFile=$(mktemp)
		trap "rm -f '$OutputFile'" EXIT
		wget --timeout=10 -O "$OutputFile" "http://plutohome.com/GetInfraredCodes.php?psc_mods=${url_psc_mods}"
		echo "http://plutohome.com/GetInfraredCodes.php?psc_mods=${url_psc_mods}"

		Header=$(head -1 "$OutputFile")
		Footer=$(tail -1 "$OutputFile")
		#[ "$Header" != "-- Database import" || "$Footer" != "-- EOF" ]] || break;

		PSC=$(head -2 "$OutputFile" | tail -1)

		if [[ "$PSC" != "" ]] ;then
			Q="UPDATE InfraredGroup_Command SET psc_mod = 0 WHERE psc_id IN ($PSC)"
			RunSQL "$Q"
		fi

		JobDone="true"
	fi
fi

## If the job is not done
if [[ "$JobDone" != "true" ]] ;then
        ## Make sure that we are runned in cron hourly, too :)
        if [[ ! -f /etc/cron.hourly/$MyName ]] ;then
                cp /usr/share/pluto-upgrade-helper/$MyName /etc/cron.hourly/
        fi

## If the job is done
else
        ## Remove us from cron.hourly
        rm -rf /etc/cron.hourly/$MyName
fi
