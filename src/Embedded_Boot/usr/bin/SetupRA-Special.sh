#!/bin/ash

cronCmd="/usr/bin/SetupRemoteAccess.sh"
cronCmd_Special="/usr/bin/SetupRA-Special.sh"
cronEntry="*/1 * * * * root ash -c '$cronCmd >/dev/null 2>&1'"
cronEntry_Special="*/10 * * * * root ash -c '$cronCmd_Special >/dev/null 2>&1'"
screenName="RemoteAssistance"

UpdateInterval=86400 #seconds
RaPass=$(cat /etc/pluto/ra_pass 2>/dev/null)

GetPorts()
{
	Q="
		SELECT IK_DeviceData
		FROM Device_DeviceData
		WHERE
			FK_Device='$PK_Device'
			AND FK_DeviceData='$DEVICEDATA_Remote_Assistance_Ports'
	"
	Result=$(cat /etc/pluto/ra_ports 2>/dev/null)
	if [[ "$Result" != *';'* ]]; then
		Result="$Result;time=0"
	fi
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
			eval "PortNoMon_${Port#nomon_}"
			;;
		*)
			eval "Port_$Port"
			;;
	esac
done
IFS="$OldIFS"

if [[ ! -L "$cronCmd_Special" ]]; then
	rm -f "$cronCmd_Special"
	ln -s "$cronCmd" "$cronCmd_Special"
fi

RAKey="/etc/pluto/ra_key"
[[ -f "$RAKey" ]] && chmod 700 "$RAKey" || exit


AddCronEntry()
{
	# if this script is not in a cron job
	#if ! grep -qF "$cronCmd" /etc/crontab; then
		# add it to crontab
	#	echo "$cronEntry" >>/etc/crontab
	#	invoke-rc.d cron reload
	#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Added crontab entry"
	#else
	#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry already present. Not adding."
	#fi

	if [[ ! -e /etc/cron.d/SetupRemoteAccess ]] ;then
		echo "$cronEntry" >>/etc/cron.d/SetupRemoteAccess
		invoke-rc.d cron reload
		echo "Added crontab entry"				
	else
		echo "Crontab entry already present. Not adding."
	fi
	
	#if ! grep -qF "$cronCmd_Special" /etc/crontab; then
	#	# add it to crontab
	#	echo "$cronEntry_Special" >>/etc/crontab
	#	invoke-rc.d cron reload
	#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Added crontab entry (special)"
	#else
	#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry (special) already present. Not adding."
	#fi

	if [[ ! -e /etc/cron.d/SetupRA-Special ]] ;then
		echo "$cronEntry_Special" >>/etc/cron.d/SetupRA-Special
		invoke-rc.d cron reload
		echo "Added crontab entry (special)"
	else
		echo "Crontab entry (special) already present. Not adding."
	fi
}

DelCronEntry()
{
	local CronReload

	# remove script from crontab
	#if grep -qF "$cronCmd" /etc/crontab; then
	#	grep -vF "$cronCmd" /etc/crontab >/etc/crontab.$$
	#	mv /etc/crontab.$$ /etc/crontab
	#	CronReload="1"
	#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry found. Removed."
	#else
	#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry not found. Not removing."
	#fi
	
	if [[ -e /etc/cron.d/SetupRemoteAccess ]] ;then
		rm -f /etc/cron.d/SetupRemoteAccess
		CronReload="1"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry found. Removed."		 
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry not found. Not removing."
	fi
	
	#if grep -qF "$cronCmd_Special" /etc/crontab; then
	#	grep -vF "$cronCmd_Special" /etc/crontab >/etc/crontab.$$
	#	mv /etc/crontab.$$ /etc/crontab
	#	CronReload="1"
	#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry (special) found. Removed."
	#else
	#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry (special) not found. Not removing."
	#fi

	if [[ -e /etc/cron.d/SetupRA-Special ]] ;then
	    rm -f /etc/cron.d/SetupRA-Special
	    CronReload="1"
	    echo "Crontab entry (special) found. Removed."
	else
	    echo "Crontab entry (special) not found. Not removing."
	fi
													

	if [[ -n "$CronReload" ]]; then
		invoke-rc.d cron reload
	fi
}

CreateTunnel()
{
	Suffix="$1"
	LocalPort="$2"
	RemotePort="$3"
	Host="$4"
	RAhost="${5:-rassh.linuxmce.org}"
	RAport="$6"
	Monitored="${7:-yes}"

	Tunnel=$(echo /var/run/screen/S-root/*.${screenName}_${Suffix})

	# if tunnel is not active create it
	Dead=0
	if [[ "$Monitored" == yes && "$RA_CheckRemotePort" == "1" ]]; then
		if [[ -z "$Tunnel" ]] || ! nc -z -w1 "$RAhost" "$RemotePort"; then
			Dead=1
		fi
	else
		if [[ -z "$Tunnel" ]]; then
			Dead=1
		fi
	fi
	if [[ "$Dead" -eq 1 ]]; then
		FalseAlarm=0
		if [[ -n "$Tunnel" && "$RA_CheckRemotePort" == "1" && "$Monitored" == yes ]]; then
			for i in $(seq 1 5); do
				/usr/bin/NetworkTest "$RAhost" "$RemotePort" >/dev/null 2>&1 && FalseAlarm=1 && break
				sleep 1
			done
		fi
		if [[ "$FalseAlarm" -eq 0 ]]; then
			[[ -n "$Tunnel" ]] && RemoveTunnel "$Suffix"
			screen -d -m -S "${screenName}_${Suffix}" /usr/bin/RemoteAccess_Tunnel.sh "$RemotePort" "$LocalPort" "$RAKey" "$Host" "$RAhost" "$RAport"
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
	Tunnel=$(echo /var/run/screen/S-root/*${screenName}_${Suffix})

	# kill port forward if it exists
	if [[ -n "$Tunnel" ]]; then
		PID="${Tunnel##*/}"
		PID="${PID%%.*}"
		kill $PID
		echo "$Suffix tunnel found. Closed."
	else
		echo "$Suffix tunnel not found. Not closing."
	fi
}

CreateTunnels()
{
	/usr/bin/RA_ChangePassword.sh

	local Var PortName PortTunnel PortNameDest PortDest

	Vars=$(env | grep ^Port_=)
	for Var in $Vars; do
		PortNameDest="${Var#Port_}"
		PortNameDest="${PortNameDest%=*}"
		PortName="${PortNameDest%_*}"
		PortDest="${PortNameDest##*_}"
		PortTunnel="${Var%*=}"
		CreateTunnel "${PortName}_pf" "$PortDest" "$PortTunnel"
	done

	CreateTunnels_Special
}

CreateTunnels_Special()
{
	local Var PortName PortTunnel PortNameDest PortDest

	Vars=$(env | grep ^PortNoMon_=)
	for Var in $Vars; do
		PortNameDest="${Var#PortNoMon_}"
		PortNameDest="${PortNameDest%=*}"
		PortName="${PortNameDest%_*}"
		PortDest="${PortNameDest##*_}"
		PortTunnel="${Var%*=}"
		CreateTunnel "${PortName}_NoMon_pf" "$PortDest" "$PortTunnel" "" "" 22 no
	done
}

RemoveTunnels()
{
	local Var PortName PortNameDest

	Vars=$(env | grep ^Port_=)
	for Var in $Vars; do
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

	Vars=$(env | grep ^PortNoMon_=)
	for Var in $Vars; do
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
	if [[ -n "$remote" ]]; then
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
