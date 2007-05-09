#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

echo "Powerfile Detection script queue <$1> <$2> <$3> <$4> <$5> <$6> <$7>"
DevSg="$5"; DevSgEsc="${DevSg//\//\/}"

type=$(lsscsi -g|awk "/$DevSgEsc"'$/ {print $2}')
manufacturer=$(lsscsi -g|awk "/$DevSgEsc"'$/ {print $3}')
model=$(lsscsi -g|awk "/$DevSgEsc"'$/ {print $4}')

echo "type $type"
if [[ "$type" == 'mediumx' && "$manufacturer" == 'PowrFile' && "$model" == 'C200' ]]; then
	echo "it's a c200"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1737
elif [[ "$type" == 'mediumx' && "$manufacturer" == 'PowrFile' && "$model" == 'C200' ]]; then
	echo "it's a sony"
else
	echo "none of the above"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
fi
