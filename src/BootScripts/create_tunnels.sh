#!/bin/base

. /usr/pluto/bin/Config_Ops.sh

# Check /etc/pluto.conf for an entry TunnelPort=x  x will be the port on plutohome.com
# Check /etc/pluto.conf for an entry TunnelPassword=x  x will be the password for the 'remote' user

if [ "$TunnelPort" = "" -o "$TunnelPassword" = "" ]; then
	userdel remote
	killall processes
	remote check_tunnel from the cronjob
else
	adduser remote
	passwd remote $TunnelPassword

	TUNNEL_KEY='pf.plutohome.com ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAIEAvLbEDtN4WmblkAzME1h74u2dELShrrH+5aduvEJ6O3gBPJwHaVgVjXH4nS6ETh/2v4bDnCwC0QOkdYbpOFEgi/25pj50pI+6DUfX33adAeUR4wtbO4BGziF2JOBUwvzRsO6JCO+FCr3Bs/SpUEzE1dp7qbdnlwV4GVKVgdqdPmM='

	if ! grep 'pf\.plutohome\.com' /root/.ssh/known_hosts; then
		echo "$TUNNEL_KEY" >> /root/.ssh/known_hosts
	fi
	screen -d -m -S SSH_Tunnel_plutohome ssh -g -R $TunnelPort:localhost:22 problems@pf.plutohome.com
	screen -d -m -S SSH_Tunnel_plutohome ssh -g -R $TunnelPort+1:localhost:80 problems@pf.plutohome.com

	add check_tunnel to the cronjob
fi
