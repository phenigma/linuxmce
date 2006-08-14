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

MyName="002-000-040-UsersIRandGSDcodes.sh"

JobDone="false"

if [[ -f /usr/pluto/bin/SQL_Ops.sh ]] ;then
        . /usr/pluto/bin/SQL_Ops.sh


	Q="
		SELECT DISTINCT 
			FK_DeviceTemplate,
			DeviceTemplate.FK_InfraredGroup,
			DeviceTemplate.FK_Manufacturer,
			DeviceTemplate.FK_DeviceCategory,
			FK_CommMethod 
		FROM
			InfraredGroup_Command 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate 
		WHERE 
			InfraredGroup_Command.FK_InfraredGroup IS NULL 
			AND
			InfraredGroup_Command.psc_mod != 0
	"
	R=$(RunSQL "$Q")

	for Record in $R ;do
		DT=$(Field "1" "$Record")
		IRG=$(Field "2" "$Record")
		M=$(Field "3" "$Record")
		DC=$(Field "4" "$Record")
		CM=$(Field "5" "$Record")

		if [[ $IRG != "NULL" ]] ;then
			Q="UPDATE InfraredGroup_Command SET InfraredGroup = $IRG WHERE FK_DeviceTemplate = $DT"
			RunSQL "$Q"
		else
			Q="INSERT INTO InfraredGroup_Command (Description,FK_Manufacturer,FK_DeviceCategory,FK_CommMethod) VALUES ('Custom codes',$M,$DC,$CM); SELECT LAST_INSERT_ID()"
			InsertID=$(RunSQL "$Q")

			Q="UPDATE InfraredGroup_Command SET InfraredGroup = $InsertID WHERE FK_DeviceTemplate = $DT"
		fi
	done

	JobDone="true"
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
