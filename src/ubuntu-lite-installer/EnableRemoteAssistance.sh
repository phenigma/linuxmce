#!/bin/bash


function generatePassword {
	local i
	local password=""
	for i in 1 2 3 4 5 6 ;do
		password="$password"$((RANDOM % 10))
	done

	echo $password
}

function validateRemotePort {
	local remote_port="$1"
	
	if [[ -z "$remote_port" ]] ;then
		return 1
	fi
	if [[ -n "${remote_port//[0-9]}" ]]; then
		return 1
	fi

	return 0
}


ra_password=$(generatePassword)
ra_remote_port=""
ra_local_port="22"

## Request the password and set the port
clear 
echo "Linux MCE DVD Remote Assistance"
echo "==============================="
echo
echo "To enable remote assistance you need to give the 'Request ID' to LinuxMCE support"
echo "and type the 'Confirmation ID' that it'll be assigned to you then press <ENTER>"
echo
echo
echo "Request ID: ${ra_password}"
while ! validateRemotePort "$ra_remote_port" ;do
	echo -n "Confirmation ID : "
	read ra_remote_port
done

echo 
echo
echo "Status :"

## Create the 'remote' user with the new passsword
echo -n " * Updating remote assistance user ... "
deluser remote &>/dev/null 
useradd -u 0 -g 0 -o -d /root -s /bin/bash -c "Remote Assistance Account" -p "$(mkpasswd -H md5 "$ra_password" 'SaltSalt')" remote &>/dev/null
ret=$?
if [[ "$ret" != 0 ]] ;then
	echo "fail"
	exit 1
fi
	echo "ok"

## Start the local ssh server
echo -n " * Starting ssh server ... "
/etc/init.d/ssh restart &>/dev/null
ret=$?
if [[ "$ret" != "0" ]] ;then
	echo "fail"
	exit 2
fi
echo "ok"

## Copy ssh key localy
mkdir -p /root/.ssh
cp /cdrom/lmce-image/pf-linuxmce /root/.ssh/
chmod 600 /root/.ssh/pf-linuxmce

## Connect to remote assistance server
connected_to_80="false"
#echo
#echo  "Connection on port 80 :"
#echo "  80 ... "
#ssh -o "StrictHostKeyChecking no" -p 80 -i /root/.ssh/pf-linuxmce -R${ra_remote_port}:localhost:22 "pf@radvd.linuxmce.com" 2>/dev/null
#ret=$?
#if [[ "$ret" != "0" ]] ;then
#	connected_to_80="false"
#else
#	connected_to_80="true"
#fi

connected_to_22="false"
echo 
echo "Connection on port 22 : "
ssh -o "StrictHostKeyChecking no" -p 22 -i /root/.ssh/pf-linuxmce -R${ra_remote_port}:localhost:22 "pf@radvd.linuxmce.com" 2>/dev/null 
ret=$?
if [[ "$ret" != "0" ]] ;then
	connected_to_22="falsea"
else
	connected_to_22="true"
fi

if [[ "$connected_to_80" == "false" && "$connected_to_22" == "false" ]] ;then
	echo "FAILED TO ENABLE REMOTE ASSISTANCE"
	exit 3
fi
