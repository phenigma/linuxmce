#!/bin/bash

shopt -s nullglob

ERR_OK=0
ERR_UNKNOWN_REP_TYPE=1
ERR_UNKNOWN_REPOS_SRC_FORM=2
ERR_APT=3
ERR_DOWNLOAD=4
ERR_DPKG_INSTALL=5
ERR_MAKE=6
ERR_UNPACK=7

SPACE_SED='s/  */ /g; s/^ *//g; s/ *$//g'

PKG_NAME=$(echo "$1" | sed "$SPACE_SED") # Package Name (includes version restriction)
REPOS_SRC=$(echo "$2" | sed "$SPACE_SED") # repository source (URL, freeform)
REPOS=$(echo "$3" | sed "$SPACE_SED") # repository (woody, sarge, ...)
REPOS_TYPE=$(echo "$4" | sed "$SPACE_SED") # repository type (numeric: 1 - package, 2 - link to source, 3 - archive)
MIN_VER=$(echo "$5" | sed "$SPACE_SED") # minimum version (text)
BIN_EXEC=$(echo "$6" | sed "$SPACE_SED") # binary executables
SRC_INCLUDES=$(echo "$7" | sed "$SPACE_SED") # source includes
SRC_IMPL=$(echo "$8" | sed "$SPACE_SED") # source implementation
BIN_LIB=$(echo "$9" | sed "$SPACE_SED") # binary library
CONFIG=$(echo "${10}" | sed "$SPACE_SED") # configuration
USERNAME=$(echo "${11}" | sed "$SPACE_SED") # CVS/SVN username
PASSWORD=$(echo "${12}" | sed "$SPACE_SED") # CVS/SVN password
PARAMETER=$(echo "${13}" | sed "$SPACE_SED") # Parameter
PK_PACKAGE="${14}"

# format, when including version restriction:
## package_name (>= version)
if [[ "$PKG_NAME" == *'('*')' ]]; then
	# package name includes version restriction
	MIN_VER="${PKG_NAME#* (}" # strip package name and opening paranthesis
	MIN_VER="${MIN_VER%)}" # strip closing paranthesis
	MIN_VER="${MIN_VER##* }" # strip comparison sign, leave just the version
	PKG_NAME="${PKG_NAME%% (*}" # strip version restriction from name
fi

CatMessages()
{
	local M="(No message)"
	if [ "$#" -gt 0 ]; then
		M="$1"
		shift
	fi

	while [ "$#" -gt 0 ]; do
		M="$(printf "%s\n\n%s" "$M" "$1")"
		shift
	done
	echo "$M"
}

# ask "Try again?" question; return 1 on "Nn", 0 otherwise
try_again()
{
	ClearBlue
	echo "$(CatMessages "$@")" | fmt
	echo -n "Do you want to try again? (Y/n): "
	read again
#	again=$(QuestionBox "$@" "Do you want to try again?")
	[ "$again" == "N" -o "$again" == "n" ] && return 1
	ClearBlack
	return 0
}

Ask()
{
	local Msg="$1"
	echo -n "$Msg: " >/dev/tty
	read Answer
	echo "$Answer"
}

NewDev()
{
	echo "Create device: $*" >/dev/stderr
	/usr/pluto/bin/CreateDevice "$@" | tee /dev/stderr | tail -1
}

# TODO: make these work at install time (currently they mess up debconf)
# the perl scripts that do this read the process stderr directly for the result
InputBox()
{
	whiptail --inputbox "$(CatMessages "$@")" 0 0 --fb --nocancel --title Pluto 2>&1 1>/dev/tty
}

MessageBox()
{
	whiptail --msgbox "$(CatMessages "$@")" 0 0 --fb --title Pluto 2>&1 1>/dev/tty
}

QuestionBox()
{
	whiptail --yesno "$(CatMessages "$@")" 0 0 --fb --title Pluto 1>/dev/tty 2>/dev/null && echo "y" || echo "n"
}
# TODO: ends here

ClearBlack()
{
	echo "[0;40m"
	clear
}

ClearBlue()
{
	echo "[0;44m"
	clear
}

PackageIsInstalled()
{
	local Pkg="$1"

	[[ -z "$Pkg" ]] && return 1
	Pkg=${Pkg//+/\\+}
	awk "/^Package: $Pkg\$/,/^\$/" /var/lib/dpkg/status | grep -q '^Status: install ok installed'
}
