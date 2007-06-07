#!/bin/bash
set -e

UPDATES_XML_URI="http://10.0.0.83/updates.xml"
UPDATES_DIR="/var/pluto-updates"
POUT=101
PIN=100

#POUT=1
#PIN=0

## Usefull functions
function Debug {
	echo "$(date -R) $*" >&2
}

function Receive {
	local msg
	read msg <&$PIN
	Debug "RECV : $msg"
	echo "$msg"
}

function Send {
	local msg="$*"
	Debug "SENT : $msg"
	echo "$msg" >&$POUT
}

function Param {
	local param_no="$1"
	local line="$2"

	echo "${line}" | cut -d" " -f${param_no}
}

function GetValue {
	local list="$2"
	local name="$1"

	# remove the spaces from in front/after the = sign
	list=$(echo $list | sed 's/ *= *"/="/g')

	echo $(eval "$list"; echo ${!name})
}


## Protocol functions
function Download {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
		return
	fi

	local update_url="$2"
	if [[ "$update_url" == "" ]] ;then
		Send "FAIL Url is empty"
		return
	fi

	local update_md5="$3"
	if [[ "$update_md5"  == "" ]] ;then
		Send "FAIL MD5 is missing"
		return
	fi

	local update_file=$(basename "$update_url")
	if [[ "$update_file" == "" ]] ;then
		Send "FAIL Cannot extract filename from url"
		return
	fi

	mkdir -p "${UPDATES_DIR}/${update_no}"
	
	wget -c --tries=5 --timeout=5 -O "${UPDATES_DIR}/${update_no}/${update_file}" "${update_url}"

	if [[ ! -f "${UPDATES_DIR}/${update_no}/${update_file}" ]] ;then
		Send "FAIL No file can be downloaded"
		return
	fi

	if [[ "$(md5sum "${UPDATES_DIR}/${update_no}/${update_file}" | cut -d' ' -f1)" != "${update_md5}" ]] ;then
		Send "FAIL MD5 sum does not match"
		return
	fi

	Send "OK"
}

function ValidateUpdate {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
		return
	fi

	mkdir -p "${UPDATES_DIR}/${update_no}"
	echo -n > "${UPDATES_DIR}/${update_no}/update.xml.progress"
	if [[ ! -f "${UPDATES_DIR}/${update_no}/update.xml.progress" ]] ;then
		Send "FAIL Cannot create update.xml file"
		return
	fi

	local finished="false"
	while [[ "$finished" != "true" ]] ;do
		local line="$(Receive)"
		if [[ "$line" != "EOF" ]] ;then
			echo "$line" >> "${UPDATES_DIR}/${update_no}/update.xml.progress"
		else
			finished="true"
		fi
	done

	mv "${UPDATES_DIR}/${update_no}/update.xml.progress" "${UPDATES_DIR}/${update_no}/update.xml"
	Send "OK"
}

function CheckUpdate {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
		return
	fi

	if [[ -f "${UPDATES_DIR}/${update_no}/update.xml" ]] ;then
		return "OK"
	else
		return "FAIL Update is not downloaded"
	fi
}

function UpdateXml {
:	
}

function Apply {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
		return
	fi

	local update_url="$2"
	if [[ "$update_url" == "" ]] ;then
		Send "FAIL Url is empty"
		return
	fi

	local update_file=$(basename "$update_url")
	if [[ "$update_file" == "" ]] ;then
		Send "FAIL Cannot extract filename from url"
		return
	fi

	if [[ ! -f "${UPDATES_DIR}/${update_no}/${update_file}" ]] ;then
		Send "FAIL Cannot find update file"
		return
	fi
	
	local update_values="$3"
	if [[ "$update_values" == "" ]];then
		Send "FAIL Can't find information on how to apply update"
		return
	fi

	case "$(echo `GetValue "action" "$update_values"` | tr "[:lower:]" "[:upper:]")" in
		"DEB")
			ApplyDeb "${UPDATES_DIR}/${update_no}/${update_file}" "${update_values}" || return
		;;
		"UNTAR")
			ApplyUntar "${update_values}" || return
		;;
	esac

	Send "OK"
}

function ApplyDeb {
	local file="$1"
	local values="$2"
	
	if  ! dpkg -i "$file" ;then
		Send "FAIL Cannot install deb \"$file\""
		return 1
	fi

	return 0
}

function ApplyUntar {
:
}

## Message loop
while /bin/true ;do
	line=$(Receive)
	if [[ "$line" == "" ]] ;then
		exit
	fi
	command=$(Param 1 "$line")

	case "${command}" in
		"UPDATE_XML")
			# UPDATE_XML
			UpdateXml
			;;
		"DOWNLOAD")
			# DOWNLOAD <UPDATE_ID> <URL> <MD5>
			Download "$(Param 2 "$line")" "$(Param 3 "$line")" "$(Param 4 "$line")"
			;;
		"VALIDATE_UPDATE")
			# VALIDATE_UPDATE <UPDATE_ID>	
			# line 1
			# ............
			# line n
			# EOF
			ValidateUpdate "$(Param 2 "$line")"
			;;
		"CHECK_UPDATE")
			# CHECK_UPDATE <UPDATE_ID>
			CheckUpdate "$(Param 2 "$line")"
			;;
		"APPLY")
			# APPLY <UPDATE_ID> <URL> [param1=value1 param2=value2 ... paramN=valueN]
			Apply "$(Param 2 "$line")" "$(Param 3 "$line")" "$(Param 4-999 "$line")"
			;;
		*)
			Debug "FAIL Unkown Command"
			;;
	esac
done
