#!/bin/ash

cronCmd="/usr/bin/SetupRemoteAccess.sh"
cronCmd_Special="/usr/bin/SetupRA-Special.sh"
cronEntry="*/1 * * * * root ash -c '$cronCmd >/dev/null 2>&1' #RA_std"
cronEntry_Special="*/10 * * * * root ash -c '$cronCmd_Special >/dev/null 2>&1' #RA_special"

UpdateInterval=86400 #seconds
RaPass=$(cat /etc/pluto/ra_password 2>/dev/null)

GetPorts()
{
	Result=$(cat /etc/pluto/ra_ports 2>/dev/null)
	case "$Result" in
		*';'*) : ;;
		*) Result="$Result;time=0" ;;
	esac
	Ports="${Result%%;*}"
	Vars="${Result#*;}"
}

GetPorts
OldIFS="$IFS"
IFS=,
for Var in $Vars; do
	case "$Var" in
		time=*)
			LastUpdate="${Var#time=}"
			Now="$(date +%s)"
			if expr $Now - $LastUpdate '<' 0 >/dev/null || expr $Now - $LastUpdate '>' $UpdateInterval >/dev/null; then
				/usr/bin/RA-handler.sh --confirm
				GetPorts
			fi
		break
		;;
	esac
done

for Port in $Ports; do
	case "$Port" in
		nomon_*)
			VarsPortNoMon="${VarsPortNoMon} PortNoMon_${Port#nomon_}"
			;;
		*)
			VarsPortStd="${VarsPortStd} Port_$Port"
			;;
	esac
done
IFS="$OldIFS"

if [[ ! -L "$cronCmd_Special" ]]; then
	rm -rf "$cronCmd_Special"
	ln -s "$cronCmd" "$cronCmd_Special"
fi

RAKey="/etc/pluto/ra_key"
[[ -f "$RAKey" ]] && chmod 700 "$RAKey" || exit


AddCronEntry()
{
	local CronReload
	
	# if this script is not in a cron job
	if ! grep -qF "#RA_std" /etc/crontabs/root 2>/dev/null; then
		# add it to crontab
		echo "$cronEntry" >>/etc/crontabs/root
		CronReload=1
		echo "Added crontab entry"
	else
		echo "Crontab entry already present. Not adding."
	fi

	if ! grep -qF "#RA_special" /etc/crontabs/root; then
		# add it to crontab
		echo "$cronEntry_Special" >>/etc/crontabs/root
		CronReload=1
		echo "Added crontab entry (special)"
	else
		echo "Crontab entry (special) already present. Not adding."
	fi
	
	if [[ -n "$CronReload" ]]; then
		/etc/init.d/cron restart
	fi
}

DelCronEntry()
{
	local CronReload

	# remove script from crontab
	if grep -qF "#RA_std" /etc/crontabs/root; then
		sed -ir '/#RA_std/d' /etc/crontabs/root
		CronReload="1"
		echo "Crontab entry found. Removed."
	else
		echo "Crontab entry not found. Not removing."
	fi
	
	if grep -qF "#RA_special" /etc/crontabs/root; then
		sed -ir '/#RA_special/d' /etc/crontabs/root
		CronReload="1"
		echo "Crontab entry (special) found. Removed."
	else
		echo "Crontab entry (special) not found. Not removing."
	fi

	if [[ -n "$CronReload" ]]; then
		/etc/init.d/cron restart
	fi
}

CreateTunnel()
{
	Suffix="$1"
	LocalPort="$2"
	RemotePort="$3"
	Host="$4"
	RAhost="${5:-cisco.fiire.com}"
	RAport="$6"
	Monitored="${7:-yes}"

	Tunnel=$(cat /tmp/tunnel.$Suffix 2>/dev/null)
	if [[ -n "$Tunnel" ]] && [[ ! -d /proc/"$Tunnel" ]]; then
		rm -f /tmp/tunnel.$Suffix
		Tunnel=
	fi

	# if tunnel is not active create it
	Dead=0
	if [[ "$Monitored" == yes ]] && [[ "$RA_CheckRemotePort" == "1" ]]; then
		if [[ -z "$Tunnel" ]] || ! /usr/bin/NetworkTest "$RAhost" "$RemotePort"; then
			Dead=1
		fi
	else
		if [[ -z "$Tunnel" ]]; then
			Dead=1
		fi
	fi
	if [[ "$Dead" -eq 1 ]]; then
		FalseAlarm=0
		if [[ -n "$Tunnel" ]] && [[ "$RA_CheckRemotePort" == "1" ]] && [[ "$Monitored" == yes ]]; then
			for i in $(seq 1 5); do
				/usr/bin/NetworkTest "$RAhost" "$RemotePort" >/dev/null 2>&1 && FalseAlarm=1 && break
				sleep 1
			done
		fi
		if [[ "$FalseAlarm" -eq 0 ]]; then
			[[ -n "$Tunnel" ]] && RemoveTunnel "$Suffix"
			/usr/bin/RemoteAccess_Tunnel.sh "$RemotePort" "$LocalPort" "$RAKey" "$Host" "$RAhost" "$RAport" &
			echo $! >/tmp/tunnel.$Suffix
			echo "$Suffix tunnel enabled."
		else
			echo "$Suffix tunnel not down. False alarm"
		fi
	else
		echo "$Suffix tunnel already present. Not enabling."
	fi
}

RemoveTunnel()
{
	Suffix="$1"
	Tunnel=$(cat /tmp/tunnel.$Suffix 2>/dev/null)
	if [[ -n "$Tunnel" ]] && [[ ! -d /proc/"$Tunnel" ]]; then
		rm -f /tmp/tunnel.$Suffix
		Tunnel=
	fi

	# kill port forward if it exists
	if [[ -n "$Tunnel" ]]; then
		AllTunnels=$(ps|awk '/RemoteAccess_Tunnel.sh/ && !/awk/ {print $1}')
		for PID in $AllTunnels; do
			PPID=$(awk '/^PPid:/ {print $2}' /proc/$PID/status)
			if [[ "$PPID" == "$Tunnel" ]]; then
				kill "$PID"
				break
			fi
		done
		kill $Tunnel
		echo "$Suffix tunnel found. Closed."
	else
		echo "$Suffix tunnel not found. Not closing."
	fi
}

CreateTunnels()
{
	/usr/bin/RA_ChangePassword.sh

	local Var PortName PortTunnel PortNameDest PortDest

	for Var in $VarsPortStd; do
		PortNameDest="${Var#Port_}"
		PortNameDest="${PortNameDest%=*}"
		PortName="${PortNameDest%_*}"
		PortDest="${PortNameDest##*_}"
		PortTunnel="${Var#*=}"
		CreateTunnel "${PortName}_pf" "$PortDest" "$PortTunnel"
	done

	CreateTunnels_Special
}

CreateTunnels_Special()
{
	local Var PortName PortTunnel PortNameDest PortDest

	for Var in $VarsPortNoMon; do
		PortNameDest="${Var#PortNoMon_}"
		PortNameDest="${PortNameDest%=*}"
		PortName="${PortNameDest%_*}"
		PortDest="${PortNameDest##*_}"
		PortTunnel="${Var#*=}"
		CreateTunnel "${PortName}_NoMon_pf" "$PortDest" "$PortTunnel" "" "" 22 no
	done
}

RemoveTunnels()
{
	local Var PortName PortNameDest

	for Var in $VarsPortStd; do
		PortNameDest="${Var#Port_}"
		PortName="${PortNameDest%_*}"
		RemoveTunnel "${PortName}_pf"
	done

	RemoveTunnels_Special
	
	/usr/bin/RA_ChangePassword.sh
}

RemoveTunnels_Special()
{
	local Var PortName PortNameDest

	for Var in $VarsPortNoMon; do
		PortNameDest="${Var#PortNoMon_}"
		PortName="${PortNameDest%_*}"
		RemoveTunnel "${PortName}_NoMon_pf"
	done
}

DeleteHostKey()
{
	sed -ri '/cisco\.fiire\.com/d' /root/.ssh/known_hosts
}

Me="$(basename "$0")"

if [[ "$Me" == "$(basename "$cronCmd")" ]]; then
	DeleteHostKey
	if [[ -n "$RaPass" ]]; then
		/usr/bin/RA_KeepPortsAlive.sh
		AddCronEntry
		[[ "$1" == "restart" ]] && RemoveTunnels
		CreateTunnels
	elif [[ -f /etc/pluto/ra_password ]]; then
		echo "Remote assistance is enabled in pluto.conf, yet remote password is empty. Avoiding closing the tunnels."
	else
		DelCronEntry
		RemoveTunnels
	fi
elif [[ "$Me" == "$(basename "$cronCmd_Special")" ]]; then
	RemoveTunnels_Special
	CreateTunnels_Special
fi
