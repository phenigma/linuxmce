#!/bin/bash

#Pass="-pPUT_PASSWORD_HERE"
MySQL="mysql -u root $Pass pluto_website -N"

Q="SELECT Description,EmailField FROM MailingList"
R=$(echo "$Q;" | $MySQL | tr ' \n\t' '~ ,')
L=$(./maillists.sh view lists | tr ' \n' '~ ')

for ListSource in $R; do
	SrcDescription=$(echo $ListSource | tr '~' ' ' | cut -d, -f1)
	Email=$(echo $ListSource | tr '~' ' ' | cut -d, -f2)
	SrcName=$(echo ${Email%@*} | tr 'A-Z' 'a-z')

	[[ "$SrcName" == "mailman" ]] && continue
	Found=0

	for ListDest in $L; do
		ListDest=$(echo "$ListDest" | tr '~' ' ')
		DestName=${ListDest% - *}
		DestName=$(echo "$DestName" | sed 's/^ *//' | tr 'A-Z' 'a-z')
		DestDescription=${ListDest#* - }

		[[ "$SrcName" == "$DestName" ]] && Found=1 && break
	done

	if [[ "$Found" -eq 0 ]]; then
		echo "New list: $SrcName"
		/usr/pluto/bin/maillists.sh new "$SrcName" "$SrcDescription"
	fi
done

for ListDest in $L; do
	ListDest=$(echo "$ListDest" | tr '~' ' ')
	DestName=${ListDest% - *}
	DestName=$(echo "$DestName" | sed 's/^ *//' | tr 'A-Z' 'a-z')
	DestDescription=${ListDest#* - }

	[[ "$DestName" == "mailman" ]] && continue
	Found=0

	for ListSource in $R; do
		SrcDescription=$(echo $ListSource | tr '~' ' ' | cut -d, -f1)
		Email=$(echo $ListSource | tr '~' ' ' | cut -d, -f2)
		SrcName=$(echo ${Email%@*} | tr 'A-Z' 'a-z')

		[[ "$SrcName" == "$DestName" ]] && Found=1 && break
	done
	
	if [[ "$Found" -eq 0 ]]; then
		echo "Deleted list: $DestName"
		/usr/pluto/bin/maillists.sh delete "$DestName"
	fi
done
