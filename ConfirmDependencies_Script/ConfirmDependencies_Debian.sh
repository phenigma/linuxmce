#!/bin/bash

. /usr/pluto/install/Common.sh

SECTIONS="main non-free contrib"

#status=$(dpkg -l "$PKG_NAME" | grep '^ii' | awk '{print $1}') #| while read status; do break; done
#if [ "$status" == "ii" ]; then
#	echo "Package is installed"
#	exit 0
#fi

case "$REPOS_TYPE" in
	# Package (APT, http/ftp: .deb)
	1) ;;
	
	# CVS (Concurrent Versioning System)
	3) URL_TYPE=cvs ;;
	
	# SVN (SubVersioN)
	4) URL_TYPE=svn ;;
	
	# HTTP/FTP download (archives: .tar.gz, .zip)
	6) ;;
	
	# Other (not supported)
	*)
		echo "TODO: implement repository type $REPOS_TYPE handling"
		exit $ERR_UNKNOWN_REP_TYPE
	;;
esac

if [ -z "$URL_TYPE" ]; then
	case "$REPOS_SRC" in
		deb*) URL_TYPE=apt; REPOS_SRC="$(echo $REPOS_SRC | sed "$SPACE_SED")" ;;
		http://*|ftp://*) URL_TYPE=direct ;;
		*) [ -z "$URL_TYPE" ] && echo "Unknown URL form in '$REPOS_SRC'" && exit $ERR_UNKNOWN_REPOS_SRC_FORM ;;
	esac
fi

# could be replaced with a "</dev/zero" for each command...
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
		echo "Repository test string: '$REPOS_SRC.+$REPOS.+$SECTIONS'"
		results=$(cat /etc/apt/sources.list | sed "$SPACE_SED" | egrep -v "^#" | egrep -c "$REPOS_SRC.+$REPOS.+$SECTIONS" 2>/dev/null)
		if [ "$results" -eq 0 ]; then
			echo "$REPOS_SRC $REPOS $SECTIONS" >>/etc/apt/sources.list
			apt-get update
		fi
		keep_sending_enters | apt-get -t "$REPOS" -y install "$PKG_NAME" || exit $ERR_APT
	;;
	
	direct)
		mkdir -p /usr/pluto/download
		/usr/pluto/install/Download_Direct.sh "$@"
		
		if [ "$PARAMETER" == ".zip" ]; then
			(cd /usr/pluto/download; unzip "${PKG_NAME}_$MIN_VER$PARAMETER")
		elif [ "$PARAMETER" == ".tar.gz" ]; then
			(cd /usr/pluto/download; tar -xzvf "${PKG_NAME}_$MIN_VER$PARAMETER")
		fi
		
		if [ "$REPOS_TYPE" -eq 1 ]; then
			keep_sending_enters | dpkg -i /usr/pluto/download/"$PKG_NAME" || exit $ERR_DPKG_INSTALL
		elif [ "$REPOS_TYPE" -eq 6 ]; then
			# source code ; how do I know where it unpacked?
			true
		fi
	;;

	svn)
		/usr/pluto/install/Download_SVN.sh "$@" || exit $?
	;;

	cvs)
		/usr/pluto/install/Download_CVS.sh "$@" || exit $?
	;;
esac
