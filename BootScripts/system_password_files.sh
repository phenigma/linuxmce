#!/bin/bash
# set system password files

RunSQL()
{
	local Q
	Q="$*"
	echo "$Q;" | /usr/bin/mysql pluto_main | tail +2 | tr '\t\n' ', '
}

cp /usr/pluto/templates/group /etc/group.$$
cp /usr/pluto/templates/passwd /etc/passwd.$$
cp /usr/pluto/templates/shadow /etc/shadow.$$
cp /usr/pluto/templates/smb.conf.tmpl /etc/samba/smb.conf.$$
cp /usr/pluto/templates/smbpasswd.tmpl /etc/samba/smbpasswd.$$

# get a list of all users
Q="SELECT PK_Users,UserName FROM Users"
R=$(RunSQL "$Q")
LinuxUserID=1001
SambaPassword=609FCABC7B0F9AEAAAD3B435B51404EE:DDFF3B733E17BE6500375694FE258864
LinuxPassword=

user_dirs="data data/movies data/pictures data/music data/documents data/home~videos data/play~lists data/tv~shows"

for Users in $R; do
	PlutoUserID=$(echo "$R" | cut -d, -f1)
	UserName=$(echo "$R" | cut -d, -f2)
	
	SambaShare="[$UserName]
	comment = $Username's private files
	browseable = yes
	writable = yes
	create mask = 0770
	directory mask = 0770
	path = /home/user_$PlutoUserID
	public = no
"
	SambaEntry="$UserName:$LinuxUserID:$SambaPassword:[U          ]:LCT-00000000:,,,"
	ShadowEntry="$UserName:$LinuxPassword:12221:0:99999:7:::"
	PasswdEntry="$Username:x:$LinuxUserID:$LinuxUserID:,,,:/home/user_$PlutoUserID:/bin/false"
	GroupEntry="$Username:x:$LinuxUserID:www-data"
	
	echo "$SambaShare" >>/etc/samba/smb.conf.$$
	echo "$SambaEntry" >>/etc/samba/smbpasswd.$$
	echo "$ShadowEntry" >>/etc/shadow.$$
	echo "$PasswdEntry" >>/etc/passwd.$$
	echo "$GroupEntry" >>/etc/group.$$

	mkdir -m 0770 /home/user_$PlutoUserID
	for dir in $user_dirs; do
		mkdir -m 0770 ${dir/~/ }
	done
	ln -sf /home/user_$PlutoUserID /home/$UserName
	adduser $UserName public
	chown -R $UserName.$UserName /home/user_$PlutoUserID

	LinuxUserID=$((LinuxUserID+1))
done

static_dirs="/home/public /home/public/data /home/public/data/movies /home/public/data/pictures /home/public/data/music /home/public/data/documents /home/temp_pvr /home/mydvd /home/cameras /home/public/data/tv~shows /home/public/data/home~videos /home/public/data/play~lists /home/tv_listing"
for dir in $static_dirs; do
	mkdir -m 0755 ${dir/~/ }
done

# Get a list of all Media Directors
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=28"
R=$(RunSql "$Q")

for Device in $R; do
	MD=$(echo "$R" | cut -d, -f1)
	mkdir -m 0755 /home/tmp_$MD
done

files="samba/smb.conf samba/smbpasswd shadow passwd group"
for file in $files; do
	mv -f /etc/$file.$$ /etc/$file
done

