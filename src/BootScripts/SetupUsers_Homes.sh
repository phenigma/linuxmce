#!/bin/bash
# set system password files

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

TemplateDir=/usr/pluto/templates

BaseDir="/home"
MakeUsers=yes
while [[ "$#" -gt 0 ]]; do
	case "$1" in
		-e) ExtraDir="$2"; ExtraLink="$3"; shift; shift ;;
		-b) BaseDir="$2"; shift ;;
		-n) MakeUsers="no" ;;
	esac
	shift
done

if [[ "$MakeUsers" == yes ]]; then
	# Extract non-pluto entries
	awk '!/^pluto_/' /etc/group >/etc/group.$$
	awk '!/^pluto_/' /etc/passwd >/etc/passwd.$$
	awk '!/^pluto_/' /etc/shadow >/etc/shadow.$$

	cp $TemplateDir/smb.conf.tmpl /etc/samba/smb.conf.$$
	cp $TemplateDir/smbpasswd.tmpl /etc/samba/smbpasswd.$$
	: >/etc/samba/usermap.txt
fi

# get a list of all users
Q="SELECT PK_Users,UserName,Password_Unix,Password_Samba FROM Users"
R=$(RunSQL "$Q")
LinuxUserID=10000
DefaultSambaPassword=609FCABC7B0F9AEAAAD3B435B51404EE:DDFF3B733E17BE6500375694FE258864
DefaultLinuxPassword=

global_static_dirs="public temp_pvr mydvd cameras tv_listing"
user_dirs="movies pictures music documents videos"
user_static_dirs="data"

UserList=

for Users in $R; do
	PlutoUserID=$(Field 1 "$Users")
	UserName=$(Field 2 "$Users" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
	LinuxPassword=$(Field 3 "$Users")
	SambaPassword=$(Field 4 "$Users")

	if [[ ! -d "$BaseDir/user_$PlutoUserID" ]]; then
		rm -rf "$BaseDir/user_$PlutoUserID"
	fi
	
	[[ -z "$LinuxPassword" ]] && LinuxPassword="$DefaultLinuxPassword"
	[[ -z "$SambaPassword" ]] && SambaPassword="$DefaultSambaPassword"
#	echo "$Users : $PlutoUserID - $UserName - $LinuxPassword - $SambaPassword"
	
	if [[ "$MakeUsers" == yes ]]; then
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
	fi

	mkdir -p -m 0770 "$BaseDir/user_$PlutoUserID"

	for dir in $user_static_dirs; do
		mkdir -p -m 0770 "$BaseDir/user_$PlutoUserID/${dir/~/ }"
	done
	
	for dir in $user_dirs; do
		mkdir -p -m 0770 "$BaseDir/user_$PlutoUserID/data/${dir/~/ }"
		if [[ -n "$ExtraDir" ]]; then
			Target="$ExtraDir/user_$PlutoUserID/data/${dir/~/ }/NAS_$ExtraLink"
			rm -f "$Target"
			ln -sf "$BaseDir/user_$PlutoUserID/data/${dir/~/ }" "$Target"
		fi
	done

	rm -f "$BaseDir/$UserName"
	ln -sf "user_$PlutoUserID" "$BaseDir/$UserName"
	UserList="$UserList $UserName"
# Files not updated at this point (woking on copies)
#	chown -R $UserName.$UserName /home/user_$PlutoUserID

	((LinuxUserID++))
done

for dir in $global_static_dirs; do
	mkdir -p -m 0755 "$BaseDir/${dir/~/ }"
done

for dir in $user_static_dirs; do
	mkdir -p -m 0755 "$BaseDir/public/${dir/~/ }"
done

for dir in $user_dirs; do
	mkdir -p -m 0755 "$BaseDir/public/data/${dir/~/ }"
	if [[ -n "$ExtraDir" ]]; then
		Target="$ExtraDir/public/data/${dir/~/ }/NAS_$ExtraLink"
		rm -f "$Target"
		ln -sf "$BaseDir/public/data/${dir/~/ }" "$Target"
	fi
done

# Get a list of all Media Directors
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=28"
R=$(RunSQL "$Q")

for Device in $R; do
	mkdir -p -m 0755 /home/tmp_$Device
done

if [[ "$MakeUsers" == yes ]]; then
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
fi

chmod -R 2770 "$BaseDir"/user_*
chmod -R 2775 "$BaseDir"/public
chgrp -R public "$BaseDir"/public
