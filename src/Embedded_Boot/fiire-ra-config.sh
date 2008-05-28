#!/bin/sh

USERNAME="$1"
PASSWORD="$2"

CONF_FILE="/etc/fiire-ra/fiire-ra.conf"
SSH_KEY_FILE="/etc/fiire-ra/keys/fiire-ra.key"

PASSWORD_MD5="`echo -n "$PASSWORD" | md5sum | cut -d' ' -f1`"

## Generate ssh keys
echo " * Generating a new ssh key"
mkdir -p "`dirname "$SSH_KEY_FILE"`"
rm -f "${SSH_KEY_FILE}.tmp"
public_key=`dropbearkey -t rsa  -f "${SSH_KEY_FILE}.tmp" 2>/dev/null | grep ^ssh-rsa`
echo "$public_key" > "${SSH_KEY_FILE}.tmp.pub"

## Post the ssh-key/username/password-md5 to the website
echo " * Sending information to Fiire server"
HTTP_Response=`curl -k -F "username=$USERNAME" -F "password=$PASSWORD_MD5" -F "keyfile=@${SSH_KEY_FILE}.tmp.pub" "https://remote.fiire.com/uploadkey.php" 2>/dev/null`

## Verify the response 
case "$HTTP_Response" in
	'FAIL:'*)
		echo "ERROR : " `echo $HTTP_Response | cut -d':' -f2`
		exit 1
	;;
	'OK:'*)
		PORT=`echo $HTTP_Response | cut -d':' -f2`
	;;
	*)
		echo "ERROR: Fiire remote server is not available"
		exit 2
	;;
esac

## Updating the configuration
echo " * Updating Remote Access Configuration"
mv "$SSH_KEY_FILE.tmp" "$SSH_KEY_FILE"
mv "$SSH_KEY_FILE.tmp.pub" "$SSH_KEY_FILE.pub"

echo "#!/bin/bash"                  > "$CONF_FILE"
echo "USERNAME=$USERNAME"  >> "$CONF_FILE"
echo "PASSWORD=$PASSWORD"  >> "$CONF_FILE"
echo "PORT=$PORT"          >> "$CONF_FILE"

chmod -R 700 "`dirname "$SSH_KEY_FILE"`"

echo " * Restarting Connection Daemon"
pid=`ps  | grep 'fiire-ra-daemon.sh'|grep -v grep | awk '{print $1}'`
if [[ "$pid" != "" ]] ;then
	kill "$pid"
fi

/usr/bin/fiire-ra-daemon.sh
