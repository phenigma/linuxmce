#!/bin/bash

if [[ "$#" -ne 2 ]]; then
	echo "Syntax: $(basename $0) {del|add} <list name>"
	exit
fi

Cmd="$1"
List="$2"

Dir="/var/qmail/mailnames/plutohome.com"
Prefix=".qmail-$List"

for Section in "" admin bounces confirm join leave owner request subscribe unsubscribe; do
	if [ -n "$Section" ]; then
		Section="-$Section"
		Command="$Section"
	else
		Command="post"
	fi
	if [[ "$Cmd" == "del" ]]; then
		rm -f "$Dir/$Prefix$Section"
	elif [[ "$Cmd" == "add" ]]; then
		echo "|/var/qmail/bin/preline /var/qmail/bin/mm_wrapper /var/mailman/mail/mailman $Command $List" >"$Dir/$Prefix$Section"
	fi
done
