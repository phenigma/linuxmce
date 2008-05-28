#!/bin/sh
CONF_FILE="/etc/fiire-ra/fiire-ra.conf"
SSH_KEY_FILE="/etc/fiire-ra/keys/fiire-ra.key"

PipePath="/var/run/fiire-ra-pipes"
PipeSend="$PipePath/send"
PipeRecv="$PipePath/recv"

SSHConnectionData="fiire-ra@remote.fiire.com"

InitPipes()
{
	mkdir -p "$PipePath"
	if [[ ! -p "$PipeSend" ]]; then
		rm -rf "$PipeSend"
		mkfifo "$PipeSend"
	fi
	if [[ ! -p "$PipeRecv" ]]; then
		rm -rf "$PipeRecv"
		mkfifo "$PipeRecv"
	fi
}

PortForwarding() {
	while :; do
		ssh -T -y -i "$SSH_KEY_FILE" -R "$PORT:localhost:80" "$SSHConnectionData" <"$PipeSend" >"$PipeRecv" &
		SSHPID=$!
		exec 8>"$PipeSend" 9<"$PipeRecv"

		echo "$USERNAME $PASSWORD_MD5" >&8

		wait "$SSHPID"
		exec 8>&- 9<&-
		sleep 5
	done
}

if [[ ! -f "$CONF_FILE" ]]; then
	echo "ERROR: Fiire Remote Access Service not configured."
fi

if [[ ! -f "$CONF_FILE" ]] ;then
	echo "ERROR: Failed to read configuration file."
	exit 1
fi
. "$CONF_FILE"

PASSWORD_MD5="`echo -n "$PASSWORD" | md5sum | cut -d' ' -f1`"

if [[ -z "$USERNAME" ]] || [[ -z "$PASSWORD" ]] || [[ -z "$PORT" ]]; then
	echo "ERROR: Username and password are not set in the configuration file."
fi

if [[ -z "$PORT" ]] ;then
	echo "ERROR: Port is not set in the configuration file."
fi

InitPipes

echo "Demonizing ..."
exec 0</dev/null
exec 1>/dev/null
exec 2>/dev/null

PortForwarding &
