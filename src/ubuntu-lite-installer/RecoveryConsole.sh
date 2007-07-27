#!/bin/bash


SelectedOption=""

OPTION_ENABLE_RA=1
OPTION_NETWORK_SETUP=2
OPTION_OPEN_SHELL=3
OPTION_SHUTDOWN=X


function readOption {

	clear
	echo "Linux MCE Recovery Menu"
	echo "======================="
	echo
	echo " 1 - Enable Remote Assistance"
	echo " 2 - Network Setup"
	echo " 3 - Open Shell Console"
	echo 
	echo " X - Shutdown"
	echo
	echo -n "Choice: " 

	read SelectedOption
}

while /bin/true ;do
	readOption

	case "$SelectedOption" in
		"$OPTION_ENABLE_RA")
			clear
			/usr/bin/EnableRemoteAssistance.sh
			echo -e "\n\nPress ENTER to get back to the menu ..."
			read	
			;;
		"$OPTION_NETWORK_SETUP")
			clear
			/usr/bin/NetworkSetup.sh	
			echo -e "\n\nPress ENTER to get back to the menu..."
			read	
			;;
		"$OPTION_OPEN_SHELL")
			clear
			echo
			echo "When you want to quit the shell press CTRL+D"
			echo
			bash
			echo -e "\n\nPress ENTER to get back to the menu..."
			read	
			;;
		"$OPTION_SHUTDOWN")
			clear
			reboot
			;;
	esac
done
