#!/bin/bash

function isInList 
{
	disk="$1"
	for dev in $device_list; do
		if [[ "$disk" == "$dev" ]]; then 
			echo '0'
		fi
 	done
	echo '1'
}

while read line; do 
	newline=$(echo ${line}|cut -d' ' -f4)
	if [[ "$newline" != "name" && -n "$newline" ]]; then
		device_list="$device_list""$newline "		
	fi
done < <(cat /proc/partitions)

for device in $device_list; do
	if [[ ${#device} == '3' ]]; then
		new_list="$new_list""$device "
	else
		len=$((${#device}-1))
		device=${device:0:$len}
		if [[ "$(isInList "$device")" == '1' ]]; then
			new_list="$new_list""$device "
		fi
	fi
done

for device in $new_list; do
	result=$(mdadm --examine --brief --scan /dev/$device)
	#if [[ -n $result ]]; then 
		#sunt hdd-uri care fac parte din array
	#else
	if [[ -z $result ]]; then
		result=$(mdadm --query /dev/$device | grep 'is not an md array')
		if [[ -n $result ]]; then
			size=$(fdisk -l /dev/$device | grep Disk | cut -d' ' -f3)
			free_disks="$free_disks/dev/$device,$size G;"
		fi
	fi
done

echo $free_disks
