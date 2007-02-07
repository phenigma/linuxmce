#!/bin/bash

# NOTE: This script doesn't check for validity of sources.list

# TODO: use something faster than md5sum, but to the same effect: unique hash
AptSrc_AddSource()
{
	local Source="$*"
	local type uri distribution components
	local id

	read type uri distribution components < <(echo "$Source")
	if [[ -z "$type" ]]; then
		return 1
	fi

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

	return 0
}

AptSrc_ParseSourcesList()
{
	local SourcesList="${1:-/etc/apt/sources.list}"
	local Source type uri distribution components
	local id comp Var

	AptSrc_id_list=' '
	while read Source; do
		Source="${Source%%#*}" # remove comments
		AptSrc_AddSource "$Source" || :
	done < "$SourcesList"
	AptSrc_WasParsed=AptSrc_WasParsed
}

AptSrc_WriteSourcesList()
{
	if [[ -z "$AptSrc_WasParsed" ]]; then
		AptSrc_ParseSourcesList
	fi

	local VarBase VarComp
	local id

	for id in $AptSrc_id_list; do
		VarBase="AptSrc_base_${id}"
		VarComp="AptSrc_comp_${id}"
		echo "${!VarBase} ${!VarComp}"
	done
}

if [[ -n "$AptSrc_Demo" ]]; then
	AptSrc_ParseSourcesList "${1:-./sample-sources.list}"
	AptSrc_WriteSourcesList
	echo "-------"
	AptSrc_AddSource "deb http://ftp.ro.debian.org/debian sarge main"
	AptSrc_AddSource "deb http://ftp.ro.debian.org/debian etch main"
	AptSrc_AddSource "deb http://ftp.ro.debian.org/debian sid main"
	AptSrc_AddSource "deb http://ftp.ro.debian.org/debian sid non-free"
	AptSrc_WriteSourcesList
fi
