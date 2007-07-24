#!/bin/bash

IPS="10.0.0.85 10.0.0.89 10.0.0.86"

#/usr/pluto/bin/MessageSend 10.0.0.89 -targetType template 0 26 1 67 241 1 13 "/usr/bin/wget" 51 "-l2	-nH	-r	-P	/tmp/	http://10.0.0.150/AutoTests/Scripts" 95 "-targetType template 0 26 1 67 13 \"/bin/bash\" 51 \"/tmp/AutoTests/Scripts/Stop.sh\""

for IP in $IPS ;do
/usr/pluto/bin/MessageSend $IP -targetType template 0 26 1 67 241 1 13 "/usr/bin/wget" 51 "-l2	-nH	-r	-P	/tmp/	http://10.0.0.150/AutoTests/Scripts" 95 "0 @AppDev@ 1 67 13 \"/bin/bash\" 241 1 51 \"/tmp/AutoTests/Scripts/Stop.sh\""
done
