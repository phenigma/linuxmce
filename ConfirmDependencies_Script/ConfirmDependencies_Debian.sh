#!/bin/bash

ERR_OK=0
ERR_UNKNOWN_REP_TYPE=1
ERR_UNKNOWN_REPOS_SRC_FORM=2
ERR_APT=3
ERR_DOWNLOAD=4
ERR_DPKG_INSTALL=5

SPACE_SED='s/  */ /g; s/^ *//g; s/ *$//g'

PKG_NAME=$(echo "$1" | sed "$SPACE_SED") # Package Name
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

SECTIONS="main non-free contrib"

status=$(dpkg -l "$PKG_NAME" | grep '^ii' | awk '{print $1}') #| while read status; do break; done
if [ "$status" == "ii" ]; then
	echo "Package is installed"
	exit 0
fi

case "$REPOS_TYPE" in
	# Package
	1) ;;
	
	# CVS (Concurrent Versioning System)
	3) URL_TYPE=cvs ;;
	
	# SVN (SubVersioN)
	4) URL_TYPE=svn ;;
	
	# Other (not supported)
	*)
		echo "TODO: implement repository type $REPOS_TYPE handling"
		exit $ERR_UNKNOWN_REP_TYPE
	;;
esac

case "$REPOS_SRC" in
	deb*) URL_TYPE=apt; REPOS_SRC="$(echo $REPOS_SRC | sed "$SPACE_SED")" ;;
	http://*|ftp://*) URL_TYPE=direct ;;
	*) [ -z "$URL_TYPE" ] && echo "Unknown URL form in '$REPOS_SRC'"; exit $ERR_UNKNOWN_REPOS_SRC_FORM ;;
esac

keep_sending_enters()
{
	while : ; do echo; done
}

export DEBIAN_FRONTEND=noninteractive

case "$URL_TYPE" in
	apt)
		TMP_SECT=$(echo -n "$REPOS" | cut -d' ' -sf2-)
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
		wget -t 0 -c -P /var/cache/apt/archives "$REPOS_SRC/$PKG_NAME" || exit $ERR_DOWNLOAD
		keep_sending_enters | dpkg -i /var/cache/apt/archives/"$PKG_NAME" || exit $ERR_DPKG_INSTALL
	;;

	svn|svn)
		[ -z "$SRC_IMPL" ] && SRC_IMPL="/usr/pluto/src/$PKG_NAME"
		mkdir -p "$SRC_IMPL" || exit $ERR_DOWNLOAD

		if [ "$URL_TYPE" == "svn" ]; then
			echo "$REPOS_SRC" | egrep '^svn(\+ssh)?://' &>/dev/null || REPOS_SRC="svn://$REPOS_SRC"
			svn_param="--non-interactive --no-auth-cache"
			[ -n "$USERNAME" ] && svn_param="$svn_param --username $USERNAME"
			[ -n "$PASSWORD" ] && svn_param="$svn_param --password $PASSWORD"
			svn checkout $svn_param "$REPOS_SRC/$REPOS/$PKG_NAME" "$SRC_IMPL"
		elif [ "$URL_TYPE" == "cvs" ]; then
			# TODO: CVS pserver login
			cvs -d "$REPOS_SRC" checkout -d "$SRC_IMPL"
			# TODO: CVS pserver logout
		fi

		pushd "$REPOS_SRC" &>/dev/null
		make
		popd &>/dev/null
	;;
esac
