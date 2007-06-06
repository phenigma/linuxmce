#!/bin/bash
set -e

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


## Protocol functions
function Download {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
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

	if [[ "$(md5sum "${UPDATES_DIR}/${update_no}/${update_file}")" != "${update_md5}" ]] ;then
		Send "FAIL MD5 sum does not match"
		return
	fi

	Send "OK"
}

function ValidateUpdate {
	local update_no="$1"
	if [[ "$update_no" == "" ]] ;then
		Send "FAIL Update number is empty"
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
	fi

	if [[ -f "${UPDATES_DIR}/${update_no}/update.xml" ]] ;then
		return "OK"
	else
		return "FAIL Update is not downloaded"
	fi
}

## Message loop
line=$(Receive)
command=$(Param 1 "$line")

case "${command}" in
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
	*)
		Debug "FAIL Unkown Command"
		;;
esac
