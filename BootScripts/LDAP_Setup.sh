#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

SlapdConf="/etc/ldap/slapd.conf"
SlapdInc="/etc/ldap/pluto.databases"

mkdir -p /usr/share/ldap/data

if ! grep -F "# Pluto" $SlapdConf &>/dev/null; then
	Template='# Pluto
database bdb
suffix "dc=plutohome, dc=org"
rootdn "cn=admin, dc=plutohome, dc=org"
rootpw secret
directory /usr/share/ldap/data
schemacheck on
lastmod on
index cn,sn,st eq,pres,sub

include $SlapdInc
'
	eval "echo \"$Template\"" >>"$SlapdConf"
fi

Template='database bdb
suffix "dc=$UserName, dc=plutohome, dc=org"
rootdn "cn=admin, dc=$UserName, dc=plutohome, dc=org"
rootpw secret
directory /usr/share/ldap/data
schemacheck on
lastmod on
index cn,sn,st eq,pres,sub
'

Q="SELECT UserName from Users"
R=$(RunSQL "$Q")

: >"$SlapdInc"

for User in $R; do
	UserName=$(Field 1 "$User")
	eval "echo \"$Template\"" >>"$SlapdInc"
done


