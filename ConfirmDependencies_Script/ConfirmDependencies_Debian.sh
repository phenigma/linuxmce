#!/bin/bash

ERR_OK=0
ERR_UNKNOWN_REP_TYPE=1
ERR_UNKNOWN_REPOS_SRC_FORM=2
ERR_APT=3
ERR_WGET=4
ERR_DPKG_INSTALL=5

PKG_NAME="$1" # Package Name
REPOS_SRC="$2" # repository source (URL, freeform)
REPOS="$3" # repository (woody, sarge, ...)
REPOS_TYPE="$4" # repository type (numeric: 1 - package, 2 - link to source, 3 - archive)
MIN_VER="$5" # minimum version (text)
BIN_EXEC="$6" # binary executables
SRC_INCLUDES="$7" # source includes
SRC_IMPL="$8" # source implementation
BIN_LIB="$9" # binary library
CONFIG="${10}" # configuration

SECTIONS="main non-free contrib"

status=$(dpkg -l "$PKG_NAME" | grep '^ii' | awk '{print $1}') #| while read status; do break; done
if [ "$status" == "ii" ]; then
	echo "Package is installed"
	exit 0
fi

case "$REPOS_TYPE" in
	1) ;;
	*)
		echo "TODO: implement repository type $REPOS_TYPE handling"
		exit $ERR_UNKNOWN_REP_TYPE
	;;
esac

SPACE_SED='s/  */ /g; s/^ *//g; s/ *$//g'

case "$REPOS_SRC" in
	deb*) URL_TYPE=apt; REPOS_SRC="$(echo $REPOS_SRC | sed "$SPACE_SED")" ;;
	http://*|ftp://*) URL_TYPE=direct ;;
	*) echo "Unknown URL form in '$REPOS_SRC'"; exit $ERR_UNKNOWN_REPOS_SRC_FORM ;;
esac

keep_sending_enters()
{
	while : ; do echo; done
}

case "$URL_TYPE" in
	apt)
		TMP_SECT=$(echo -n "$REPOS" | cut -d' ' -f2-)
		if [ -n "$TMP_SECT" ]; then
			SECTIONS="$TMP_SECT"
			REPOS=$(echo -n "$REPOS" | cut -d' ' -f1)
		fi
		if ! cat /etc/apt/sources.list | sed "$SPACE_SED" | egrep "$REPOS_SRC.+$REPOS.+$SECTIONS" 2>/dev/null; then
			echo "$REPOS_SRC $REPOS $SECTIONS" >>/etc/apt/sources.list
			apt-get update
		fi
		keep_sending_enters | apt-get -t "$REPOS" -y install "$PKG_NAME" || exit $ERR_APT
	;;
	direct)
		wget -t 0 -c -P /var/cache/apt/archives "$REPOS_SRC/$PKG_NAME" || exit $ERR_WGET
		keep_sending_enters | dpkg -i /var/cache/apt/archives/"$PKG_NAME" || exit $ERR_DPKG_INSTALL
	;;
esac
