#!/bin/bash

Device="$1"

exec 3>&1 1>&2

URL="http://freedb.freedb.org/~cddb/cddb.cgi"
ProtoVersion=5
User=pluto
Host=$(hostname)

Err_DiscID=1
Err_Query=2
Err_NoMatch=3

Err=0
Tab="$(echo -e "\t")"

output()
{
	echo "$@" >&3
}

# Debug:
#DiscID="a00e8b0b 11 150 13097 30565 46492 64745 81322 104112 121970 145060 164240 196365 3725" # single-artist / multi-match
#DiscID="b909530f 15 150 10838 21155 32670 42753 54848 68235 79165 89028 99405 109753 127673 138285 148088 166775 2389" # multi-artist / single-match
DiscID="$(/usr/bin/cd-discid "$Device")" || exit $Err_DiscID

TrackCount=$(echo "$DiscID" | cut -d' ' -f2)
Tag="$DiscID${Tab}Unknown Artist${Tab}Unknown Title${Tab}"
for ((i = 1; i <= $TrackCount; i++)); do
	Tag="$Tag${Tab}Track $i"
done

if ! Query="$(/usr/bin/cddb-tool query "$URL" "$ProtoVersion" "$User" "$Host" "$DiscID")"; then
	Err=$Err_Query
else
	Code=$(echo "$Query" | head -1 | cut -d' ' -f1)
	case "$Code" in
		200) # one match
			QueryID=$(echo "$Query" | cut -d' ' -f2,3)
		;;
		202|403|409|503) # no match/error
			Err=$Err_NoMatch
		;;
		210|211) # multiple match (210 - exact, 211 - inexact)
			QueryID=$(echo "$Query" | head -2 | tail -1 | cut -d' ' -f1,2)
		;;
	esac
fi

[[ $Err -ne 0 ]] && output "$Tag" && exit

/usr/bin/cddb-tool read "$URL" "$ProtoVersion" "$User" "$Host" "$QueryID" >/tmp/cddbread.$$
Read="$(/usr/bin/cddb-tool parse /tmp/cddbread.$$)"
rm -f /tmp/cddbread.$$
eval "$Read"

Tag="$DiscID$Tab$DARTIST$Tab$DALBUM$Tab$CDGENRE"
#SlashCount=0
#DashCount=0
#ParenCount=0
#for ((i = 1; i <= $TrackCount; i++)); do
#	T="$(eval echo "\${TRACK$i}")"
#	case "$T" in
#		*/*) ((SlashCount++)) ;;
#		*-*) ((DashCount++)) ;;
#		*\(*\)*) ((ParenCount++)) ;;
#	esac
#done
#
#Half=$((TrackCount / 2))
#Separator=
#[[ $SlashCount -gt $Half ]] && Separator='/'
#[[ $DashCount -gt $Half ]] && Separator='-'
#[[ $ParentCount -gt $Half ]] && Separator='(' #()

for ((i = 1; i <= $TrackCount; i++)); do
	Title="$(eval echo \$TRACK$i)"
	[[ -z "$Title" ]] && Title="Track $i"
	Tag="$Tag$Tab$Title"
done
output "$Tag"
