#!/bin/bash

. /usr/pluto/bin/pluto.func

if [[ "$#" -ne 3 ]]; then
	Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Called with wrong number of parameters: $#"
	exit 1
fi

RemotePort="$1"
LocalPort="$2"
RAKey="$3"

if [ -f "$RAKey" ]; then
	chmod 700 "$RAKey"
	chown root.root "$RAKey"
else
	Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Key '$RAKey' not found. Exiting"
	exit 1
fi

# TODO: check or remove host key every time just in case
while echo 'x'; do
	sleep 5
done |
	ssh -i $RAKey -R$RemotePort:localhost:$LocalPort remoteassistance@pf.plutohome.com |
	tee /dev/tty | /usr/pluto/bin/charperiod
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Tunnel to port $LocalPort died"
