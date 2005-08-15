#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

cronCmd="/usr/pluto/bin/SetupRemoteAccess.sh"
cronCmd_Special="/usr/pluto/bin/SetupRA-Special.sh"
cronEntry="*/1 * * * * root $cronCmd"
cronEntry_Special="*/10 * * * * root $cronCmd_Special"
screenName="RemoteAssistance"

PortBase=$((PK_Installation*4+10000))

if [[ ! -L "$cronCmd_Special" ]]; then
	rm -f "$cronCmd_Special"
	ln -s "$cronCmd" "$cronCmd_Special"
fi

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
	
	if ! grep -qF "$cronCmd_Special" /etc/crontab; then
		# add it to crontab
		echo "$cronEntry_Special" >>/etc/crontab
		/etc/init.d/cron reload
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Added crontab entry (special)"
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry (special) already present. Not adding."
	fi
}

DelCronEntry()
{
	local CronReload

	# remove script from crontab
	if grep -qF "$cronCmd" /etc/crontab; then
		grep -vF "$cronCmd" /etc/crontab >/etc/crontab.$$
		mv /etc/crontab.$$ /etc/crontab
		CronReload="1"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry found. Removed."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry not found. Not removing."
	fi
	
	if grep -qF "$cronCmd_Special" /etc/crontab; then
		grep -vF "$cronCmd_Special" /etc/crontab >/etc/crontab.$$
		mv /etc/crontab.$$ /etc/crontab
		CronReload="1"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry (special) found. Removed."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry (special) not found. Not removing."
	fi

	if [[ -n "$CronReload" ]]; then
		/etc/init.d/cron reload
	fi
}

CreateTunnel()
{
	Suffix="$1"
	LocalPort="$2"
	RemotePort="$3"
	Host="$4"
	RAhost="$5"
	RAport="$6"
	Monitored="${7:-yes}"

	Tunnel=$(echo /var/run/screen/S-root/*${screenName}_${Suffix}*)

	# if tunnel is not active create it
	Dead=0
	if [[ "$Monitored" == yes && "$RA_CheckRemotePort" == "1" ]]; then
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
			screen -d -m -S "${screenName}_${Suffix}" /usr/pluto/bin/RemoteAccess_Tunnel.sh "$RemotePort" "$LocalPort" "$RAKey" "$Host" "$RAhost" "$RAport"
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
	Tunnel=$(echo /var/run/screen/S-root/*${screenName}_${Suffix})

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

	CreateTunnel "SSH" 22 $PortBase
	CreateTunnel "Web" 80 $((PortBase+1))
	CreateTunnels_Special
}

CreateTunnels_Special()
{
	CreateTunnel "SSH_NoMon_pf" 22 $((PortBase+2)) "" "" 22 no
	CreateTunnel "SSH_NoMon_ph" 22 $((PortBase+3)) "" pf2.plutohome.com 22 no
}

RemoveTunnels()
{
	RemoveTunnel "SSH"
	RemoveTunnel "Web"
	RemoveTunnels_Special
	
	if grep -q remote /etc/passwd; then
		# delete user "remote" if it exists
		userdel remote || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to remove user 'remote' with exit code '$?'"
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "User 'remote' not present. Nothing to remove"
	fi
}

RemoveTunnels_Special()
{
	RemoveTunnel "SSH_NoMon_pf"
	RemoveTunnel "SSH_NoMon_ph"
}

DeleteHostKey()
{
	sed -i '/pf2?\.plutohome\.com/d' /root/.ssh/known_hosts
}

Me="$(basename "$0")"

if [[ "$Me" == "$(basename "$cronCmd")" ]]; then
	DeleteHostKey
	if [ -n "$remote" ]; then
		AddCronEntry
		[ "$1" == "restart" ] && RemoveTunnels
		CreateTunnels
	else
		DelCronEntry
		RemoveTunnels
	fi
elif [[ "$Me" == "$(basename "$cronCmd_Special")" ]]; then
	RemoveTunnels_Special
	CreateTunnels_Special
fi
