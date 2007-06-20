#!/bin/bash

original_list="./original.list"
local_list="/tmp/local.list"
diff_list="/tmp/diff.list"
clean_system="true"

function GenerateDiff {
	while read orig_name orig_ver ;do
		local local_line=$(grep "^$orig_name " $local_list)
		if [[ "$local_line" == "" ]] ;then
			echo "MISSING '$orig_name'" > "$diff_list"
			clean_system="false"
			continue
		fi
	
		local local_name=$(echo $local_line | cut -d' ' -f1)
		local local_ver=$(echo $local_line | cut -d' ' -f2-99)
		
		if [[ "$orig_ver" != "$local_ver" ]] ;then
			echo "CHANGED '$orig_name' '$local_ver' '$orig_ver'" > "$diff_list"
			clean_system="false"
			continue
		fi
	done < "$original_list"
}

function GenerateList {
	local temp_file="$(mktemp)"
	dpkg -l | grep ^ii | awk '{print $2; print $3}' > $temp_file

	local x
	local y
	while read x && read y ;do
	        echo "$x $y" >> "$local_list"
	done <  "$temp_file"

	rm -f "$temp_file"
}

GenerateList
GenerateDiff

sleep 1
if [[ "$clean_system" == "false" ]] ;then
	exit 1
else
	echo "Everything OK" > "$diff_list"
fi
