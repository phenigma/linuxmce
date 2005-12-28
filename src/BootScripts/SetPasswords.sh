#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

PK_Users=$1
Password=$2
PasswordUnix=$(mkpasswd -H md5 "$Password")
PasswordSamba=$(/usr/pluto/bin/smbpass.pl "$Password")

sqlPassword="${Password//\'/\'}"
sqlPasswordUnix="${PasswordUnix//\'/\'}"
sqlPasswordSamba="${PasswordSamba//\'/\'}"

Q="UPDATE Users SET Password=md5('$sqlPassword'), Password_Unix='$sqlPasswordUnix', Password_Samba='$sqlPasswordSamba' WHERE PK_Users=$PK_Users"
RunSQL "$Q"

/usr/pluto/bin/SetupUsers_Homes.sh
