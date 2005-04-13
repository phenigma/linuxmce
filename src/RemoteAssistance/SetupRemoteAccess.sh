#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

cronEntry="*/10 * * * * root /usr/pluto/bin/SetupRemoteAccess.sh"
screenName="RemoteAssistance"

RAKey="/usr/pluto/keys/id_dsa_remoteassistance"
[ -f "$RAKey" ] && chmod 700 "$RAKey" || exit

shopt -s nullglob

screen -wipe &>/dev/null
CreateTunnel()
{
	SSHTunnel=$(echo /var/run/screen/S-root/*${screenName}_SSH*)
	WebTunnel=$(echo /var/run/screen/S-root/*${screenName}_Web*)

	# if user doesn't exist
	if ! grep -q remote /etc/passwd; then
		# add user "remote" w/ the password specified (var value)
		Pass=$(mkpasswd -H md5 "$remote" 'RAPlutoP')
		useradd -u 0 -g 0 -o -d /root -c "Pluto Remote Assistance Account" -p "$Pass" remote || Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Failed to add user 'remote' with exit code '$?'"
		SCRvar="export SCREENDIR=/var/run/screen/S-root"
		grep -qF "$SCRvar" /root/.bashrc || echo "$SCRvar" >>/root/.bashrc
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "User 'remote' already exists. Not adding."
	fi

	# if this script is not in a cron job
	if ! grep -qF "$cronEntry" /etc/crontab; then
		# add it to be run every 10 mins
		echo "$cronEntry" >>/etc/crontab
		/etc/init.d/cron reload
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Added crontab entry"
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry already present. Not adding."
	fi
	
	# if tunnels are not active
	if [ -z "$SSHTunnel" ]; then
		# create the ssh tunnel (port 22) w/ port=PK_Installation*2+10000
		RemotePort=$((PK_Installation*2+10000))
		screen -d -m -S "${screenName}_SSH" /usr/pluto/bin/RemoteAccess_Tunnel.sh $RemotePort 22 "$RAKey"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "SSH tunnel enabled."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "SSH tunnel already present. Not enabling."
	fi

	if [ -z "$WebTunnel" ]; then
		# create the http tunnel (port 80) w/ port=PK_Installation*2+10001
		RemotePort=$((PK_Installation*2+10001))
		screen -d -m -S "${screenName}_Web" /usr/pluto/bin/RemoteAccess_Tunnel.sh $RemotePort 80 "$RAKey"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "HTTP tunnel enabled."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "HTTP tunnel already present. Not enabling."
	fi
}

RemoveTunnel()
{
	SSHTunnel=$(echo /var/run/screen/S-root/*${screenName}_SSH*)
	WebTunnel=$(echo /var/run/screen/S-root/*${screenName}_Web*)

	if grep -q remote /etc/passwd; then
		# delete user "remote" if it exists
		userdel remote || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to remove user 'remote' with exit code '$?'"
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "User 'remote' not present. Nothing to remove"
	fi
	
	# remove script from cron job
	if grep -qF "$cronEntry" /etc/crontab; then
		grep -vF "$cronEntry" /etc/crontab >/etc/crontab.$$
		mv /etc/crontab.$$ /etc/crontab
		/etc/init.d/cron reload
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry found. Removed."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry not found. Not removing."
	fi
	
	# kill port forwards if they exist
	if [ -n "$SSHTunnel" ]; then
		PID="${SSHTunnel##*/}"
		PID="${PID%%.*}"
		kill $PID
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "SSH tunnel found. Closed."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "SSH tunnel not found. Not closing."
	fi
	
	if [ -n "$WebTunnel" ]; then
		PID="${WebTunnel##*/}"
		PID="${PID%%.*}"
		kill $PID
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "HTTP tunnel found. Closed."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "HTTP tunnel not found. Not closing."
	fi
}

if [ -n "$remote" ]; then
	[ "$1" == "restart" ] && RemoveTunnel
	CreateTunnel
else
	RemoveTunnel
fi
