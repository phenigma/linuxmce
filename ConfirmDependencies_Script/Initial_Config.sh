#!/bin/bash

echo "** Initial config script"

activation_url="http://activation.plutohome.com/a.php"
activation_url="http://10.0.0.150/a.php"

ok=0
while [ "$ok" -eq 0 ]; do
	echo -n "Please input your activation key: "
	read activation_key
	if [ -z "$activation_key" ]; then
		echo "Empty activation key. Try again"
		continue
	fi

	answer=$(wget -O - "$activation_url?code=$activation_key" 2>/dev/null)
	if [ "$?" -ne 0 ]; then
		echo "Failed to contact activation server. Try again later"
		continue
	fi
	result=$(echo "$answer" | head -1)
	if [ "$result" != "OK" ]; then
		echo "Failed getting activation data. Server answer was:"
		echo "$result"
		continue
	fi

	echo "Code accepted"
	config=$(echo "$answer" | awk 'NR>1')
	echo "TODO: parse configuration"
	echo "$config"
	ok=1
done
