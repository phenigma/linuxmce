#!/bin/bash

IPS="${1:-"10.0.1.1 10.0.1.2"}"

for IP in $IPS ;do
/usr/pluto/bin/MessageSend $IP -targetType template 0 26 1 67 241 1 13 "/usr/bin/wget" 51 "-l2	-nH	-r	-P	/tmp/	http://10.0.2.6/AutoTests/Scripts" 95 "0 @AppDev@ 1 67 13 \"/bin/bash\" 241 1 51 \"/tmp/AutoTests/Scripts/Stop.sh\""
done
