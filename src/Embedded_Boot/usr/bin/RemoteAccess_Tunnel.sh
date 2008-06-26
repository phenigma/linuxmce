#!/bin/ash

# RemotePort, LocalPort, RAKey, Host, RAhost, RAport

if [[ "$#" -lt 3 ]] || [[ "$#" -gt 6 ]]; then
	echo "Called with wrong number of parameters: $#"
	exit 1
fi

RemotePort="$1"
LocalPort="$2"
RAKey="$3"
Host="$4"
RAhost="$5"

if [[ -z "$RAhost" ]]; then
	RAhost="cisco.fiire.com"
fi

if [[ -n "$6" ]]; then
	RAport="$6"
fi

RAportTest=$(echo "$RAport" | sed -r 's/[0-9]//g')
if [[ -z "$RAport" ]] || [[ -n "$RAportTest" ]]; then
	RAport=22
fi

if [[ -z "$Host" ]]; then
	Host=localhost
fi

if [[ -n "$RAKey" ]] && [[ -f "$RAKey" ]]; then
	chmod 700 "$RAKey"
	chown root.root "$RAKey"
else
	echo "Key '$RAKey' not found. Exiting"
	exit 1
fi

# TODO: check or remove host key every time just in case
# TODO: check if port 80 isn't proxyed and if it is, also check port 22
while echo 'x'; do
	sleep 5
done |
	ssh -y -T -p $RAport -i $RAKey -R $RemotePort:$Host:$LocalPort "remoteassistance@$RAhost" | /usr/bin/charperiod
echo "Tunnel to port $LocalPort died"
