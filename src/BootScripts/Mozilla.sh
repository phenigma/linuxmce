#!/bin/bash

User="$1"
URL="$2"

if [[ ! -f ~/.mozilla/firefox/profiles.ini ]]; then
	firefox --help
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

if [[ "$Default" == 1 ]]; then
	FireFoxProfile="~/.mozilla/firefox/$Path/"
fi

echo "$(date -R) user $User URL $URL" >> /var/log/pluto/mozilla.newlog

if [[ $User != 0 ]]; then
	if [[ ! -f "/home/user_$User/bookmarks.html" ]]; then
		echo "User $User doesn't have bookmarks yet"
		cp /home/public/bookmarks.html "$FireFoxProfile"
	fi
fi

echo "starting firefox" >> /var/log/pluto/mozilla.newlog
firefox "$URL"
if [[ $User != 0 ]]; then
	cp "$FireFoxProfile/bookmarks.html" /home/user_$User
fi
echo "$(date -R) firefox ended" >> /var/log/pluto/mozilla.newlog
