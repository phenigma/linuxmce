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

TemplateDB='database bdb
suffix "dc=$UserName, dc=plutohome, dc=org"
rootdn "cn=admin, dc=$UserName, dc=plutohome, dc=org"
rootpw secret
directory /usr/share/ldap/data
schemacheck on
lastmod on
index cn,sn,st eq,pres,sub
'

TemplateTemp='
dn: uid=$UserName, ou=users, dc=plutohome, dc=org
uid: $UserName
sn: $UserName
userPassword: secret
objectClass: top
objectClass: person
objectClass: organizationalPerson
objectClass: inetOrgPerson

dn: dc=$UserName, dc=plutohome, dc=org
dc: $UserName
objectClass: top
objectClass: domain
description: $UserName
'

Q="SELECT UserName from Users"
R=$(RunSQL "$Q")

: >"$SlapdInc"
: >/tmp/data.ldap

for User in $R; do
	UserName=$(Field 1 "$User")
	eval "echo \"$TemplateDB\"" >>"$SlapdInc"
	eval "echo \"$TemplateTemp\"" >>/tmp/data.ldap
done

