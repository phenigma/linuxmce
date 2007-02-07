#!/bin/bash

# TODO: use something faster than md5sum, but to the same effect: unique hash
ParseSourcesList()
{
	local SourcesList="${1:-/etc/apt/sources.list}"
	local line type uri distribution components
	local id comp Var

	id_list=' '
	while read line; do
		line="${line%%#*}" # remove comments
		read type uri distribution components < <(echo "$line")
		[[ -z "$type" ]] && continue # empty line
		
		uri="${uri%/}/" # make sure URI ends with a slash
		id=$(echo "$type $uri $distribution" | md5sum | cut -d' ' -f1)
		if [[ "$id_list" != *" $id "* ]]; then
			id_list="$id_list$id "
			eval "base_${id}='$type $uri $distribution'"
			eval "comp_${id}=' '" # comp_* has a leading space
		fi
		for comp in $components; do
			Var="comp_${id}"
			if [[ "${!Var}" == *" $comp "* ]]; then # comp_* is bound and separated by spaces
				continue
			fi
			eval "comp_${id}=\"\${comp_${id}}$comp \"" # comp_* has a trailing space
		done
	done < "$SourcesList"
}

WriteSourcesList()
{
	local VarBase VarComp
	local id

	for id in $id_list; do
		VarBase="base_${id}"
		VarComp="comp_${id}"
		echo "${!VarBase} ${!VarComp}"
	done
}

ParseSourcesList "$1"
WriteSourcesList
