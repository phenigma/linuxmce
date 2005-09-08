#!/bin/bash

. /usr/pluto/bin/pluto.func

#$0 <MAC> <path_to_sis>

if [ "$#" -ne "2" ]; then
	echo "Usage: $0 <MAC> <path_to_sis|scan>"
	exit
fi

MAC="$1"
SIS="$2"

#TODO: check if MAC is valid

Ports=
for ((i=1;i<=10;i++)); do
	if /usr/pluto/bin/TestChannel "$MAC" $i &>/dev/null; then
		if [[ "$SIS" == scan ]]; then
			echo $i
		else
			Ports="$Ports $i"
		fi
	fi
done

if [[ "$SIS" == scan ]]; then
	exit 0
fi

if [[ ! -f "$SIS" || ! -r "$SIS" ]]; then
	echo "sis file '$SIS' not found or not accessible"
	exit
fi

Ret=1
for Port in $Ports; do
	Try=5
	echo "Trying to send the file using channel '$Port'..."
	while :; do
		Output="$( (echo "c"; echo "p"; echo "$SIS Orbiter.cab"; echo "q") | obex_test -b $MAC $Port)"
		if [[ "$Output" == *"Link broken!"* ]]; then # Not our guy
			break
		elif [[ "$Output" == *"Transport connect error! (Bluetooth)"* ]]; then # EBUSY
			((Try--))
			if ((Try == 0)); then
				ForceExit=1
				break
			fi
			sleep 1
		elif [[ "$Output" == *"PUT successful!"* ]]; then # upload ok
			Ret=0
			ForceExit=1
			break
		else # should not get here
			echo "Twilight zone. Welcome."
			break
		fi
	done
	if [[ -n "$ForceExit" ]]; then
		break
	fi
done

if [[ "$Ret" -eq 0 ]]; then
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "SIS '$SIS' uploaded to '$MAC'"
	exit 0
else
	Logging "$TYPE" "$SEVERITY_WARNING" "$0" "SIS '$SIS' failed to upload to '$MAC'"
	exit 1
fi 
