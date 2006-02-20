#!/bin/bash
# set system password files

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

function showUsage {
	echo "Usage: $0 [OPTIONS] <PlutoUserID>"
	echo 
	echo " -d		delete home directory"
	echo " -a		delete all files owned by user"
	echo " -b[=DIR]		create at backup of user files before deleting them and place it in"
	echo "                  the DIR directory. If no DIR is specified the backup should be placed"
	echo "                  /tmp directory"
}

while getopts dba name 
do
	case $name in
		d)	
			DeleteHomeDir="Yes"
		;;
		b)	
			BackupUserFiles="Yes"
			if [[ -z "$OPTARG" ]]; then
				BackupDir="/tmp"
			else
				BakupDir=$OPTARG
			fi
		;;
		a)
			DeleteAllFiles="Yes"
		;;
		?)
			showUsage
			exit 1
		;;
	esac
done

shift $(($OPTIND - 1))
PlutoUserID=$1

if [[ -z $PlutoUserID ]] ;then
	showUsage
	exit 1
fi


## Read UserName from database
Q="
	SELECT 
		UserName,
		Password_Unix,
		Password_Samba 
	FROM 
		Users 
	WHERE 
		PK_Users=$PlutoUserID
"
R=$(RunSQL "$Q")

UserName=$(Field 1 "$R" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
if [[ -z $UserName ]]; then
	echo "ERROR: Cannot find user with id $PlutoUserID"
	exit 1
fi
LinuxPassword=$(Field 2 "$R")
SambaPassword=$(Field 3 "$R")

## Read BaseDir from /etc/passwd
BaseDir=$(cat /etc/passwd | grep $UserName | cut -d':' -f6)
BaseDir=${BaseDir%/*}


## Prepare & run the deluser command
deluserOptions=""
if [[ "$DeleteHomeDir" == "Yes" ]]; then
	deluserOptions="$deluserOptions --remove-home"
fi
if [[ "$DeleteAllFiles" == "Yes" ]]; then
	deluserOptions="$deluserOptions --remove-all-files"
fi
if [[ "$BackupUserFiles" == "Yes" ]]; then
	deluserOptions="$deluserOptions --backup --backup-to $BackupDir"
fi

deluser $deluserOptions $UserName"
