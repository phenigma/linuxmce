#!/bin/bash

# NOTE: This script doesn't check for validity of sources.list

# TODO: use something faster than md5sum, but to the same effect: unique hash
ParseSourcesList()
{
	local SourcesList="${1:-/etc/apt/sources.list}"
	local line type uri distribution components
	local id comp Var

	AptSrc_id_list=' '
	while read line; do
		line="${line%%#*}" # remove comments
		read type uri distribution components < <(echo "$line")
		[[ -z "$type" ]] && continue # empty line
		
		uri="${uri%/}/" # make sure URI ends with a slash
		id=$(echo "$type $uri $distribution" | md5sum | cut -d' ' -f1)
		if [[ "$AptSrc_id_list" != *" $id "* ]]; then
			AptSrc_id_list="$AptSrc_id_list$id "
			eval "AptSrc_base_${id}='$type $uri $distribution'"
			eval "AptSrc_comp_${id}=' '" # AptSrc_comp_* has a leading space
		fi
		for comp in $components; do
			Var="AptSrc_comp_${id}"
			if [[ "${!Var}" == *" $comp "* ]]; then # AptSrc_comp_* is bound and separated by spaces
				continue
			fi
			eval "AptSrc_comp_${id}=\"\${AptSrc_comp_${id}}$comp \"" # AptSrc_comp_* has a trailing space
		done
	done < "$SourcesList"
}

WriteSourcesList()
{
	local VarBase VarComp
	local id

	for id in $AptSrc_id_list; do
		VarBase="AptSrc_base_${id}"
		VarComp="AptSrc_comp_${id}"
		echo "${!VarBase} ${!VarComp}"
	done
}

ParseSourcesList "$1"
WriteSourcesList
