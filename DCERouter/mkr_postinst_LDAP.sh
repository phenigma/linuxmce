#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

SlapdConf="/etc/ldap/slapd.conf"
#SlapdInc="/etc/ldap/pluto.databases"

mkdir -p /usr/share/ldap/data

ConfAppend="# Pluto
database bdb
suffix \"dc=plutohome, dc=org\"
rootdn \"cn=admin, dc=plutohome, dc=org\"
rootpw secret
directory /usr/share/ldap/data
schemacheck on
lastmod on
index cn,sn,st eq,pres,sub


access to dn=\".*dc=([^,]+),dc=plutohome,dc=org\"
       by dn=\"uid=\$1,ou=users,dc=plutohome,dc=org\" write
       by * read

access to *
       by * read

access to *
       by dn=\"cn=admin,dc=plutohome,dc=org\" write
       by dn=\"cn=admin,dc=plutohome,dc=org\" read

"

grep -F "# Pluto" "$SlapdConf" >/dev/null || echo "$ConfAppend" >> "$SlapdConf"

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
cn: $UserName
userPassword: {MD5}$UserPassword
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
		
ldapadd -f /tmp/data.ldap -xvc -D "cn=admin,dc=plutohome,dc=org" -w secret

