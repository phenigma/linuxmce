#!/bin/bash

names_list=$(ls /dev | grep md)

if [[ -z $names_list ]]; then 
	echo "md0"
	exit
fi

max=0
for name in $names_list; do
	nr=${name:2}
	if [[ $nr > $max ]]; then
		max=$nr
	fi
done

max=$(($max+1))

echo "md$max"

