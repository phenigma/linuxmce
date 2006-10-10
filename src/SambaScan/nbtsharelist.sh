#!/bin/bash

echo "1$1"
echo "2$2"
echo "3$3"


n=0
c=0
smbclient -U $1 -L $2 | while read line
do
	if [ "$line" = "session setup failed: NT_STATUS_LOGON_FAILURE" ]; then
		echo "Bad password."
	else
		if ((n > 2)) 
		then
			arrresult=($line)
			if [ ${#arrresult[*]} -gt 1 ]; then 
				if [ ${arrresult[1]} = "Disk" ]; then
					((++c))
					echo "${arrresult[0]}"
				fi
			fi 
		else
		     ((++n))
		fi
	fi
done
if [ $c -eq 0 ]; then
	echo "No shares found, password required?"
fi


		
	        
