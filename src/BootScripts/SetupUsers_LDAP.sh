#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

TemplateTemp='
dn: uid=$UserName, ou=users, dc=plutohome, dc=org
uid: $UserName
sn: $UserName
cn: $UserName
userPassword: $UserName
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

Q="SELECT UserName, Password from Users"
R=$(RunSQL "$Q")

#: >"$SlapdInc"
: >/tmp/data.ldap

echo "dn: dc=plutohome, dc=org
dc: plutohome
objectClass: top
objectClass: domain

dn: ou=users, dc=plutohome, dc=org
ou: users
description: These are privileged users
objectClass: top
objectClass: organizationalUnit

dn: cn=admin, dc=plutohome, dc=org
objectClass: organizationalRole
cn: admin
description: LDAP Directory Administrator
" >> /tmp/data.ldap


for User in $R; do
	UserName=$(Field 1 "$User")
	UserPassword=$(Field 2 "$User")
#	eval "echo \"$TemplateDB\"" >>"$SlapdInc"
	eval "echo \"$TemplateTemp\"" >>/tmp/data.ldap
done

/etc/init.d/slapd restart

timeout=60
waited=0
while ! nc -z localhost 389 &>/dev/null && [ "$waited" -lt "$timeout" ]; do
	sleep 1
	waited=$((waited + 1))
done
		
ldapadd -c -f /tmp/data.ldap -xvc -D "cn=admin,dc=plutohome,dc=org" -w secret &> /dev/null || true
