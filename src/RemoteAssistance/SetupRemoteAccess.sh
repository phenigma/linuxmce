#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

cronCmd="/usr/pluto/bin/SetupRemoteAccess.sh"
cronEntry="*/1 * * * * root $cronCmd"
screenName="RemoteAssistance"

RAKey="/usr/pluto/keys/id_dsa_remoteassistance"
[ -f "$RAKey" ] && chmod 700 "$RAKey" || exit

shopt -s nullglob

screen -wipe &>/dev/null

AddCronEntry()
{
	# if this script is not in a cron job
	if ! grep -qF "$cronCmd" /etc/crontab; then
		# add it to crontab
		echo "$cronEntry" >>/etc/crontab
		/etc/init.d/cron reload
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Added crontab entry"
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry already present. Not adding."
	fi
}

DelCronEntry()
{
	# remove script from crontab
	if grep -qF "$cronCmd" /etc/crontab; then
		grep -vF "$cronCmd" /etc/crontab >/etc/crontab.$$
		mv /etc/crontab.$$ /etc/crontab
		/etc/init.d/cron reload
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry found. Removed."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry not found. Not removing."
	fi
}

CreateTunnel()
{
	Suffix="$1"
	LocalPort="$2"
	RemotePort="$3"

	Tunnel=$(echo /var/run/screen/S-root/*${screenName}_${Suffix}*)

	# if tunnel is not active create it
	Dead=0
	if [[ "$RA_CheckRemotePort" == "1" ]]; then
		if [[ -z "$Tunnel" ]] || ! nc -z -w1 pf.plutohome.com "$RemotePort"; then
			Dead=1
		fi
	else
		if [[ -z "$Tunnel" ]]; then
			Dead=1
		fi
	fi
	if [[ "$Dead" -eq 1 ]]; then
		FalseAlarm=0
		if [[ -n "$Tunnel" && "$RA_CheckRemotePort" == "1" ]]; then
			for ((i=0;i<5;i++)); do
				nc -z -w1 pf.plutohome.com "$RemotePort" && FalseAlarm=1 && break
				sleep 1
			done
		fi
		if [[ "$FalseAlarm" -eq 0 ]]; then
			[[ -n "$Tunnel" ]] && RemoveTunnel "$Suffix"
			screen -d -m -S "${screenName}_${Suffix}" /usr/pluto/bin/RemoteAccess_Tunnel.sh "$RemotePort" "$LocalPort" "$RAKey"
			Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "$Suffix tunnel enabled."
		else
			Logging "$TYPE" "$SEVERITY_WARNING" "$0" "$Suffix tunnel not down. False alarm"
		fi
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "$Suffix tunnel already present. Not enabling."
	fi
}

RemoveTunnel()
{
	Suffix="$1"
	Tunnel=$(echo /var/run/screen/S-root/*${screenName}_${Suffix}*)

	# kill port forward if it exists
	if [ -n "$Tunnel" ]; then
		PID="${Tunnel##*/}"
		PID="${PID%%.*}"
		kill $PID
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "$Suffix tunnel found. Closed."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "$Suffix tunnel not found. Not closing."
	fi
}

CreateTunnels()
{
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

	CreateTunnel "SSH" 22 $((PK_Installation*2+10000))
	CreateTunnel "Web" 80 $((PK_Installation*2+10001))
}

RemoveTunnels()
{
	RemoveTunnel "SSH"
	RemoveTunnel "Web"
	
	if grep -q remote /etc/passwd; then
		# delete user "remote" if it exists
		userdel remote || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to remove user 'remote' with exit code '$?'"
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "User 'remote' not present. Nothing to remove"
	fi
}

DeleteHostKey()
{
	sed -i '/pf\.plutohome\.com/d' /root/.ssh/known_hosts
}

DeleteHostKey
if [ -n "$remote" ]; then
	AddCronEntry
	[ "$1" == "restart" ] && RemoveTunnels
	CreateTunnels
else
	DelCronEntry
	RemoveTunnels
fi
