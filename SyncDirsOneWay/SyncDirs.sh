#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Usage: $0 <source dir> <mirror dir|ssh mirror location>"
	exit
fi

SRC_DIR="$1"
DST_DIR="$2"

SRC_LIST="/tmp/src.listing.$$"
DST_LIST="/tmp/dst.listing.$$"

if [ ! -d "$SRC_DIR" ]; then
	echo "Not a directory: $SRC_DIR"
	exit
else
#	TZ="GMT" ls -gG "$SRC_DIR" | awk '{print $4,$5,$6,$7}' >"$SRC_LIST"
	TZ="GMT" ls -1 "$SRC_DIR" >"$SRC_LIST"
fi

REMOTE_DIR="$DST_DIR"
if [ ! -d "$DST_DIR" ]; then
	if echo "$DST_DIR" | grep ':' &>/dev/null; then
		# it's probably an SSH mirror specifier: [user@]host:dir
		REMOTE_DIR=$(echo "$DST_DIR" | awk -F':' '{print $2}')
		LEFT=$(echo "$DST_DIR" | awk -F':' '{print $1}')
		if echo "$LEFT" | grep '@' &>/dev/null; then
			# contains a username
			USER=$(echo "$LEFT" | awk -F'@' '{print $1}')
			HOST=$(echo "$LEFT" | awk -F'@' '{print $2}')
			USER_PART="$USER@"
		else
			# only host name
			HOST="$LEFT"
		fi
		if ssh "$USER_PART$HOST" "[ -d \"$REMOTE_DIR\" ]"; then
#			ssh "$USER_PART$HOST" "TZ=\"GMT\" ls -gG \"$REMOTE_DIR\" | awk '{print \$4,\$5,\$6,\$7}'" >"$DST_LIST"
			ssh "$USER_PART$HOST" "TZ=\"GMT\" ls -1 \"$REMOTE_DIR\"" >"$DST_LIST"
			SCP1="s"
			SCP2="-p "
			SSH1="ssh $USER_PART$HOST '"
			SSH2="'"
		else
			echo "Not a directory: $DST_DIR"
			exit
		fi
	else
		echo "Not a directory: $DST_DIR"
		exit
	fi
else
#	TZ="GMT" ls -gG "$DST_DIR" | awk '{print $4,$5,$6,$7}' >"$DST_LIST"
	TZ="GMT" ls -1 "$DST_DIR" >"$DST_LIST"
fi

diff -u "$DST_LIST" "$SRC_LIST" | view -
#exit

diff -u "$DST_LIST" "$SRC_LIST" |
awk '
	BEGIN {removed = ""; added = ""}
	/^@@/ || NR <= 2 { next }
	{
		action = substr($0, 0, 1);
		file = substr($0, 2);
		if (action == "+")
		{
#			print("Copy to mirror:", file);
#			print("cp \"'"$SRC_DIR"'/" file "\" \"'"$DST_DIR"'\"");
			added = added "\"'"$SRC_DIR"'/" file "\" ";
		}
		else if (action == "-")
		{
#			print("Delete from mirror:", file);
#			print("rm -f \"'"$DST_DIR"'/" file "\"");
			removed = removed "\"'"$REMOTE_DIR"'/" file "\" ";
		}
#		else
#			print("No change:", file);
	}
	END {
		command = "true";
		if (added != "") added = added "\"'"$SRC_DIR"'/Packages.gz\""
		if (removed != "") command = command "; '"$SSH1"'rm -f " removed "'"$SSH2"'";
		if (added != "") command = command "; '"$SCP1"'cp '"$SCP2"'" added " \"'"$DST_DIR"'\"";
		print(command);
	}
' | bash #>/tmp/perform_sinc.$$.sh

rm -f /tmp/src.listing.$$ /tmp/dest.listing.$$
