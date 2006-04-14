#!/bin/bash

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
		-d) ((i++)); Device_ID="${Params[$i]}" ;;
		-i) ((i++)); Device_IP="${Params[$i]}" ;;
		-m) ((i++)); Device_MAC="${Params[$i]}" ;;
	esac
done

/usr/pluto/bin/StorageDevices_Setup.sh --cascade

