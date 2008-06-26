#!/bin/ash

WebServer="cisco.fiire.com"

if [[ ! -f /etc/pluto/installation_number ]]; then
	exit 1
fi

InstID=$(cat /etc/pluto/installation_number)
RaPass=$(cat /etc/pluto/ra_password 2>/dev/null)

if [[ -z "$InstID" ]]; then
	echo "Unusable installation number: '$InstID'"
	exit 1
fi

if [[ "$#" -ne 1 ]]; then
	echo "Usage: '$0' {--enable|--disable|--status|--confirm}"
	exit 1
fi

PortPage="http://$WebServer/get_ra_ports.php?installationID=$InstID"

GeneratePasswordOf6Digits()
{
	local pass
	for i in $(seq 1 6); do
		number=$RANDOM
		digit=$(expr $number % 10)
		pass="${pass}${digit}"
	done
	echo "$pass"
}

UpdatePortDeviceData()
{
	local IK_DeviceData="$1"
	local Time=$(date +%s)

	if [[ -n "$IK_DeviceData" ]]; then
		IK_DeviceData="$IK_DeviceData;time=$Time"
	fi

	echo "$IK_DeviceData" >/etc/pluto/ra_ports
}

RequestPorts()
{
	local IK_DeviceData Q password Var PortName PortValue line
	local PortFile="$(mktemp /tmp/tmp.RA-XXXXXX)"
	trap "rm -f '$PortFile'" EXIT

	if ! wget -qO "$PortFile" "$PortPage"; then
		echo "Failed to get ports from server"
		UpdatePortDeviceData ""
		return 1
	fi

	if [[ "$(head -1 "$PortFile")" != "#Ports:" ]] || [[ "$(tail -1 "$PortFile")" != "#END" ]]; then
		echo "Received incomplete response"
		UpdatePortDeviceData ""
		return 1
	fi

	IK_DeviceData=
	while read line; do
		case "$line" in
			'#'*) continue ;;
		esac
		IK_DeviceData="${IK_DeviceData}${line},"
	done <"$PortFile"
	
	IK_DeviceData="${IK_DeviceData%,}"
	UpdatePortDeviceData "$IK_DeviceData"
	return 0
}

EnableRA()
{
	if ! RequestPorts; then
		rm -f /etc/pluto/ra_password
		echo "Failed to enable Remote Assistance"
		return 1
	fi

	if [[ ! -f /etc/pluto/ra_password ]]; then
		RaPass="$(GeneratePasswordOf6Digits)"
		echo "$RaPass" >/etc/pluto/ra_password
	fi

	/usr/bin/SetupRemoteAccess.sh
}

DisableRA()
{
	local Q

	rm -f /etc/pluto/ra_password
	/usr/bin/SetupRemoteAccess.sh
	UpdatePortDeviceData ""
	wget -qO /dev/null "$PortPage&action=del" 2>/dev/null
}

ShowStatus()
{
	if [[ -n "$RaPass" ]]; then
		echo "Remote Assistance is enabled"
		return 0
	else
		echo "Remote Assistance is disabled"
		return 1
	fi
}

RetCode=0
case "$1" in
	--enable)
		EnableRA
	;;
	--disable)
		DisableRA
	;;
	--status)
		ShowStatus
		RetCode=$?
	;;
	--confirm)
		if [[ -n "$RaPass" ]]; then
			RequestPorts
			RetCode=$?
		fi
	;;
	*)
		echo "Unknown parameter: '$1'"
		RetCode=1
	;;
esac
exit $RetCode
