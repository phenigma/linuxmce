#!/bin/bash

# 

resultip=0
resultmachine=1
resultmac=2
resulttype=3

DBHostName=localhost
DBUserName=root
DBPassword=
DBName=pluto_main

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh


## 
# Scan for windows shares

## Radu/Razvan the following should not be hardcoded but rather come from the 'internal network' and the 24 is if the subnet mask is 255.255.255.0, otherwise it's 16 if it's 255.255.0.0
./nbtscan -m 192.168.2.0/24 | while read line
do
	arrresult=($line)
	if [ ${#arrresult[*]} = 4 ]; then
		
		# check to see if this IP is already known
	
		Q1="SELECT IPaddress FROM Device WHERE IPaddress='${arrresult[$resultip]}' LIMIT 1"
		Q2="SELECT IPaddress FROM UnknownDevices WHERE IPaddress='${arrresult[$resultip]}' LIMIT 1"
		Q3="SELECT IPaddress FROM PnpQueue WHERE IPaddress='${arrresult[$resultip]}' LIMIT 1"
		echo "$Q"
		
		IP=$(RunSQL "$Q1")
				
		echo "Result is $IP"

## Radu/Razvan -- this needs to run all 3 queries, and if any of the results are not empty (don't bother doing a compare, just a 'not empty', then the if is true and we will skip it

		if [ $IP = ${arrresult[$resultip]} ]; then

			echo "$IP is already in the System.";
			
		else

				/usr/pluto/bin/MessageSend 192.168.2.152 0 -1001 2 65 52 3 53 2 49 1837 5 "${arrresult[$resultmac]}" 28 "${arrresult[$resultip]}" 55 "186|${arrresult[$resultmachine]}"
		fi
		
		echo "${arrresult[$resultip]},*,${arrresult[$resultmachine]}"
		
	fi
done
