#!/bin/bash

User="$1"
URL="$2"

DefaultProfileTxt="[General]
StartWithLastProfile=1

[Profile0]
Name=default
IsRelative=1
Path=pluto.default"

if [[ ! -f ~/.mozilla/firefox/profiles.ini ]]; then
	mkdir -p ~/.mozilla/firefox/pluto.default
	echo "$DefaultProfileTxt" >~/.mozilla/firefox/profiles.ini
fi

Section=
while read line; do
	if [[ "$line" == '['Profile*']' ]]; then
		if [[ -n "$Section" && "$Default" == 1 ]]; then
			break
		fi
		Section="$line"
		Name=
		IsRelative=
		Path=
		Default=
	elif [[ -n "$Section" && -n "$line" ]]; then
		Var="${line%%=*}"
		Value="${line#*=}"
		eval "$Var='$Value'"
	fi
done <~/.mozilla/firefox/profiles.ini

if [[ -z "$Path" ]]; then
	echo "$(date -R) FATAL: Profile path is empty" >> /var/log/pluto/mozilla.newlog
	exit 1
fi
FireFoxProfile=~/".mozilla/firefox/$Path/"

echo "$(date -R) user $User URL $URL" >> /var/log/pluto/mozilla.newlog

if [[ $User != 0 ]]; then
	if [[ ! -f "/home/user_$User/bookmarks.html" ]]; then
		echo "User $User doesn't have bookmarks yet"
		cp /home/public/bookmarks.html "$FireFoxProfile"
	else
		cp "/home/user_$User/bookmarks.html" "$FireFoxProfile"
	fi
fi

echo "starting firefox" >> /var/log/pluto/mozilla.newlog
firefox "$URL"
if [[ $User != 0 ]]; then
	cp "$FireFoxProfile/bookmarks.html" /home/user_$User
fi
echo "$(date -R) firefox ended" >> /var/log/pluto/mozilla.newlog
