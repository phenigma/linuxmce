#!/bin/bash
# set system password files

. /usr/pluto/bin/pluto.func

RunSQL()
{
	local Q
	Q="$*"
	echo "$Q;" | /usr/bin/mysql pluto_main | tail +2 | tr '\t\n' ', '
}

TemplateDir=/usr/pluto/templates

# Extract non-pluto entries
awk '!/^pluto_/' /etc/group >/etc/group.$$
awk '!/^pluto_/' /etc/passwd >/etc/passwd.$$
awk '!/^pluto_/' /etc/shadow >/etc/shadow.$$

cp $TemplateDir/smb.conf.tmpl /etc/samba/smb.conf.$$
cp $TemplateDir/smbpasswd.tmpl /etc/samba/smbpasswd.$$
: >/etc/samba/usermap.txt

# get a list of all users
Q="SELECT PK_Users,UserName,Password_Unix,Password_Samba FROM Users"
R=$(RunSQL "$Q")
LinuxUserID=10000
DefaultSambaPassword=609FCABC7B0F9AEAAAD3B435B51404EE:DDFF3B733E17BE6500375694FE258864
DefaultLinuxPassword=

user_dirs="data data/movies data/pictures data/music data/documents data/videos"
UserList=

for Users in $R; do
	PlutoUserID=$(echo "$Users" | cut -d, -f1)
	UserName=$(echo "$Users" | cut -d, -f2 | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
	LinuxPassword=$(echo "$Users" | cut -d, -f3)
	SambaPassword=$(echo "$Users" | cut -d, -f4)

	[ -z "$LinuxPassword" ] && LinuxPassword="$DefaultLinuxPassword"
	[ -z "$SambaPassword" ] && SambaPassword="$DefaultSambaPassword"
#	echo "$Users : $PlutoUserID - $UserName - $LinuxPassword - $SambaPassword"
	
	SambaShare="[$UserName]
	comment = $UserName's private files
	browseable = yes
	writable = yes
	create mask = 0770
	directory mask = 0770
	path = /home/user_$PlutoUserID
	public = no
"
# TODO: replace 00000001 with a real number of seconds (never set to 00000000)
	SambaEntry="pluto_$UserName:$LinuxUserID:$SambaPassword:[U          ]:LCT-00000001:,,,"
	ShadowEntry="pluto_$UserName:$LinuxPassword:12221:0:99999:7:::"
	PasswdEntry="pluto_$UserName:x:$LinuxUserID:$LinuxUserID:,,,:/home/user_$PlutoUserID:/bin/false"
	GroupEntry="pluto_$UserName:x:$LinuxUserID:www-data"
	SambaUnixMap="pluto_$UserName = $UserName"
	
	echo "$SambaShare" >>/etc/samba/smb.conf.$$
	echo "$SambaEntry" >>/etc/samba/smbpasswd.$$
	echo "$ShadowEntry" >>/etc/shadow.$$
	echo "$PasswdEntry" >>/etc/passwd.$$
	echo "$GroupEntry" >>/etc/group.$$
	echo "$SambaUnixMap" >>/etc/samba/usermap.txt

	mkdir -p -m 0770 /home/user_$PlutoUserID
	for dir in $user_dirs; do
		mkdir -p -m 0770 /home/user_$PlutoUserID/${dir/~/ }
	done
	rm -f /home/$UserName
	ln -sf /home/user_$PlutoUserID /home/$UserName
	UserList="$UserList $UserName"
# Files not updated at this point (woking on copies)
#	chown -R $UserName.$UserName /home/user_$PlutoUserID

	LinuxUserID=$((LinuxUserID+1))
done

static_dirs="/home/public /home/public/data /home/public/data/movies /home/public/data/pictures /home/public/data/music /home/public/data/documents /home/temp_pvr /home/mydvd /home/cameras /home/public/data/videos /home/tv_listing"
for dir in $static_dirs; do
	mkdir -p -m 0755 ${dir/~/ }
done

# Get a list of all Media Directors
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=28"
R=$(RunSQL "$Q")

for Device in $R; do
	MD=$(echo "$R" | cut -d, -f1)
	mkdir -p -m 0755 /home/tmp_$MD
done

files="samba/smb.conf samba/smbpasswd shadow passwd group"
for file in $files; do
	mv -f /etc/$file.$$ /etc/$file
done

# Only now we have the copies in place of the originals so we can add users and set ownerships
addgroup public &>/dev/null
for User in $UserList; do
	adduser pluto_$User public &>/dev/null
	chown --dereference -R pluto_$User.pluto_$User /home/$User/
done

