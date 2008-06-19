#!/bin/sh

if [[ "$ACTION" == released ]]; then
	case "$BUTTON" in
		reset) /usr/bin/Pluto_Reset.sh ;;
		ses) /usr/bin/Pluto_EZSetup.sh ;;
	esac
fi
