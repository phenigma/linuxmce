#!/bin/bash

## Try to figure out what is the external network interface 
## by looking at the defaul router interface
netExtName=$(route -n | grep "^0.0.0.0" | head -1 | awk '{print $8}')

## If the external interface is still null, nada, niente, nimic
## just use the first one in the list
if [[ "$netExtName" == "" ]] ;then
	netExtName=$(ifconfig -a | grep "^eth" | awk '{ print $1 }' | head -1)
fi

## If we didn't got any external netwoking interface thill now, we might well quit
## Since is nothing that we can do
if [[ "$netExtName" == "" ]] ;then
	echo "0"
	exit 0
fi

## If we found what the internal network interface is, we should see if we cat get
## a internal one too
netIntName=$(ifconfig -a | grep "^eth" | grep -v "^${netExtName}" | awk '{print $1}' | head -1)

## If we didn't get one going, we'll use an alias for the external one
netCards=2
if [[ "$netIntName" == "" ]] ;then
	netCards=1
	netIntName="${netExtName}:0"
fi

netExtIP=$(ifconfig $netExtName | grep "inet addr" | sed 's/.*inet addr:\([0-9.]*\).*Bcast.*/\1/g')
netExtMask=$(ifconfig $netExtName | grep "inet addr" | sed 's/.*Mask:\([0-9.]*\).*$/\1/g')
netExtGate=$(route -n | grep "^0.0.0.0" | head -1 | awk '{print $2}')
netExtDNS1=$(cat /etc/resolv.conf | grep "^nameserver" | head -1 | awk '{print $2}')
netExtDNS2=$(cat /etc/resolv.conf | grep "^nameserver" | tail -1 | awk '{print $2}')

netInternetWorking=0
nc -z www.google.com 80 2>/dev/null && netInternetWorking=1

# pattern matches these situations:
# dhclient ..parms.. eth0
# dhclient3 ..parms.. eth0
# /sbin/dhclient ..parms.. eth0
# /sbin/dhclient3 ..parms.. eth0
# and so on
netUseDhcp=0
if ps ax | egrep -q " (/sbin/)?dhclient.? .*${netExtName}\$" ;then
	netUseDhcp=1
fi

echo "$netCards"
echo "$netExtName|$netIntName"
echo "$netExtIP|$netExtMask|$netExtGate|$netExtDNS1|$netExtDNS2|$netUseDhcp|$netInternetWorking"
