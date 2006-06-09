#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

## Get a list of repos
Q="SHOW TABLES LIKE 'psc_%_tables'"
R=$(RunSQL "$Q")
Repos=
for TblName in $R ;do
	Rname=$(echo $TblName | cut -d'_' -f2)	
	Repos="${Repos} ${Rname}"
done

## Get a list of tables
Q="SHOW TABLES"
R=$(RunSQL "$Q")
Tables=
for TblName in $R ;do
	Tables="${Tables} $TblName"
done

for Rname in $Repos ;do
	TablesInRepo=
	Q="SELECT Tablename,last_psc_id,last_psc_batch  FROM psc_${Rname}_tables ORDER BY Tablename";
	R=$(RunSQL "$Q")
	for Row in $R ;do
		Tablename=$(Field 1 "$Row")
		last_psc_id=$(Field 2 "$Row")
		last_psc_batch=$(Field 3 "$Row")

		## Get real psc_id and psc_batch for $Tablename
		real_psc_id=-1
		real_psc_batch=-1

		if echo " $Tables " | grep -q " $Tablename " ;then

			Q="SELECT MAX(psc_id), MAX(psc_batch) FROM ${Tablename}"
			R=$(RunSQL "$Q")
			
			max_id=$(Field 1 "$R")
			max_batch=$(Field 2 "$R")
			
			if [[ "$max_id" != "" ]] ;then
				real_psc_id=$max_id
			fi

			if [[ "$max_batch" != "" ]] ;then
				real_psc_batch=$max_batch
			fi
		fi

		## Get hist psc_id and psc_batch for $Tablename
		hist_psc_id=-1
		hist_psc_batch=-1
		
		if echo " $Tables " | grep -q " ${Tablename}_pschist " ;then
			Q="SELECT MAX(psc_id), MAX(psc_batch) FROM ${Tablename}_pschist"
			R=$(RunSQL "$Q")

			max_hist_id=$(Field 1 "$R")
			max_hist_batch=$(Field 2 "$R")

			if [[ "$max_hist_id" != "" ]] ;then
				hist_psc_id=$max_hist_id
			fi
			
			if [[ "$max_hist_batch" != "" ]] ;then
				hist_psc_batch=$max_hist_batch
			fi
		fi

		## Set psc_id/psc_batch to maximum of real/hist
		if [[ "$real_psc_id" -gt "$hist_psc_id" ]];then
			psc_id=$real_psc_id
		else
			psc_id=$hist_psc_id
		fi

		if [[ "$real_psc_batch" -gt "$hist_psc_batch" ]] ;then
			psc_batch=$real_psc_batch
		else
			psc_batch=$hist_psc_batch
		fi
		
		## Make the correction
		if [[ "$last_psc_batch" -gt 5000 ]] ;then
			if [[ "$psc_batch" == "-1" ]] ;then
				new_batch=0
			else
				new_batch=$psc_batch
			fi

			echo -n "Correcting last_psc_batch for table ${Tablename} ($last_psc_batch => $new_batch) ... ";
			Q="UPDATE psc_${Rname}_tables SET last_psc_batch=${new_batch} WHERE Tablename=${Tablename}"
			#RunSQL "$Q"
			echo "done"
		fi
	done
	
done
