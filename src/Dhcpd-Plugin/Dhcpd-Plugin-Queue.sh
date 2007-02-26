#!/bin/bash

while read mac ip garbage; do
	QueueNew=()

	for Pair in "${Queue[@]}"; do
		read Qmac Qip < <(echo "$Pair")
		if ! /usr/pluto/bin/MessageSend $DCERouter $PK_Device -1001 2 65 52 3 53 2 5 "$Qmac" 28 "$Qip"; then
			QueueNew=("${QueueNew[@]}" "$Pair")
		fi
	done
	
	Queue=("${QueueNew[@]}")

	if ! /usr/pluto/bin/MessageSend $DCERouter $PK_Device -1001 2 65 52 3 53 2 5 "$mac" 28 "$ip"; then
		Queue=(${Queue[@]} "$mac $ip")
	fi
done
