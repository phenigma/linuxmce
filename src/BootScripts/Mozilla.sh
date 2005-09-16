#!/bin/bash

User="$1"
URL="$2"
FireFoxProfile="~/.mozilla/firefox/eo8ytrya.default/" ## need help
echo "$(date -R) user $User URL $URL" >> /var/log/pluto/mozilla.newlog

if [[ $User!=0 && ! -f "/home/user_$1/bookmarks.html" ]]; then
	echo "User $User doesn't have bookmarks yet"
	cp /home/public/bookmarks.html "$FireFoxProfile"  ###  !!!!! Why doesn't this work!!!
fi

echo "starting firefox" >> /var/log/pluto/mozilla.newlog
firefox "$URL"
cp "$FireFoxProfile/bookmarks.html" /home/user_$1
echo "$(date -R) firefox ended" >> /var/log/pluto/mozilla.newlog
