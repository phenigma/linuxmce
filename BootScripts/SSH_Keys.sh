#!/bin/bash

Dir=/usr/pluto/keys
Key=id_dsa_pluto

if [ ! -f /etc/diskless.conf ] && ! [ -f $Dir/$Key -a -f $Dir/$Key.pub ]; then
	echo "Creating SSH keys"
	mkdir -p /usr/pluto/keys
	ssh-keygen -t dsa -C "Pluto auto-generated key" -f $Dir/$Key -P ""
fi

mkdir -m 0700 -p /root/.ssh
Lines=$(sed 's/#.*$//g; s/^[ \t]*//g; s/[ \t]*$//g' /etc/ssh/ssh_config | awk '!/^$/' | wc -l)
if [ "$Lines" -eq 0 ]; then
	echo "Writing defaults to ssh_config"
	cp /usr/pluto/templates/ssh_config.tmpl /etc/ssh/ssh_config
fi

for i in /etc/ssh/ssh_config /root/.ssh/config; do
	grep 'Host..*\*' "$i" &>/dev/null && HostFound=1
done

File='/root/.ssh/config'
if [ -z "$HostFound" ] || ! grep "IdentityFile $Dir/$Key" "$File" &>/dev/null; then
	HostEntry="Host *
	IdentityFile $Dir/$Key"
	echo "$HostEntry" >>"$File"
fi

KeyString="$(cat $Dir/$Key.pub)"
File='/root/.ssh/authorized_keys'
[ -f "$File" ] && grep -F "$KeyString" "$File" &>/dev/null && KeyPresent=1

[ -z "$KeyPresent" ] && cat $Dir/$Key.pub >>"$File"

/etc/init.d/ssh restart
