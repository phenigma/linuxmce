#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

cronEntry="*/10 * * * * root /usr/bin//usr/pluto/bin/SetupRemoteAccess.sh"
screenName="RemoteAssistance"

RAKey="-i /usr/pluto/keys/id_dsa_remoteassistance"

shopt -s nullglob

screen -wipe &>/dev/null
SSHTunnel=$(echo /var/run/screen/S-root/*${screenName}_SSH*)
WebTunnel=$(echo /var/run/screen/S-root/*${screenName}_Web*)
if [ -n "$remote" ]; then
	# if user doesn't exist
	if ! grep -q remote /etc/passwd; then
		# add user "remote" w/ the password specified (var value)
		Pass=$(mkpasswd -H md5 "$remote" 'RAPlutoP')
		useradd -u 0 -o -d /root -c "Pluto Remote Assistance Account" -p "$Pass" remote
	fi

	# if this script is not in a cron job
	if ! grep -qF "$cronEntry" /etc/crontab; then
		# add it to be run every 10 mins
		echo "$cronEntry" >>/etc/crontab
		/etc/init.d/cron reload
	fi
	
	# if tunnels are not active
	if [ -z "$SSHTunnel" ]; then
		# create the ssh tunnel (port 22) w/ port=PK_Installation*2+10000
		RemotePort=$((PK_Installation*2+10000))
		screen -d -m -S "${screenName}_SSH" ssh $RAKey -R$RemotePort:localhost:22 remoteassistance@pf.plutohome.com
	fi

	if [ -z "$WebTunnel" ]; then
		# create the http tunnel (port 80) w/ port=PK_Installation*2+10001
		RemotePort=$((PK_Installation*2+10001))
		screen -d -m -S "${screenName}_Web" ssh $RAKey -R$RemotePort:localhost:80 remoteassistance@pf.plutohome.com
	fi
else
	if grep -q remote /etc/passwd; then
		# delete user "remote" if it exists
		userdel remote
	fi
	
	# remove script from cron job
	if grep -qF "$cronEntry" /etc/crontab; then
		grep -vF "$cronEntry" /etc/crontab >/etc/crontab.$$
		mv /etc/crontab.$$ /etc/crontab
		/etc/init.d/cron reload
	fi
	
	# kill port forwards if they exist
	if [ -n "$SSHTunnel" ]; then
		PID="${SSHTunnel##*/}"
		PID="${PID%%.*}"
		kill $PID
	fi
	
	if [ -n "$WebTunnel" ]; then
		PID="${WebTunnel##*/}"
		PID="${PID%%.*}"
		kill $PID
	fi
fi
