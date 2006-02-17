#!/bin/bash

function getCurrentLayout {
	currentLayout=$(cat $XConfFile  | grep "^.*Option.*\"XkbLayout\"" | cut -f5 | sed 's/"//g')
	if [[ $currentLayout ]]; then
		echo $currentLayout
	else
		echo "us" #This should be the default
	fi
}

function setCurrentLayout {
	local layout=$1
	
	# Test is the Layout option is inable in X Config File
	grep -q "^.*Option.*\"XkbLayout\".*" $XConfFile
	if [[ $? -eq 0 ]]; then
		# Just replace the current layout
		sed "s|^.*Option.*\"XkbLayout\".*|\tOption\t\t\"XkbLayout\"\t\"$layout\"|g" $XConfFile > $XConfFile.$$
	else
		# Create the layout option
		sed "s|^.*Driver.*\"keyboard\"|\tDriver\t\t\"keyboard\"\n\tOption\t\t\"XkbLayout\"\t\"$layout\"|g" $XConfFile > $XConfFile.$$
	fi
	
	mv $XConfFile.$$ $XConfFile
}

Action=$1
IP=$2
KbLayout=$3

# Where is the X Config stored
if [[ "$IP" != "127.0.0.1" ]]; then 
	XConfPath="/usr/pluto/diskless/$IP"
else
	XConfPath=""
fi

XConfFile="$XConfPath/etc/X11/XF86Config-4"
case "$Action" in
	get)
		getCurrentLayout
	;;

	set)
		setCurrentLayout $KbLayout
		if [[ "$IP" != "127.0.0.1" ]]; then
			ssh $IP "setxkbmap $KbLayout" &>/dev/null
		else
			setxkbmap $KbLayout 2>/dev/null
		fi
	;;

	*)
		echo "Usage $0 <Action> <IP> [KbLayout]"
		exit 0 
	;;
esac
