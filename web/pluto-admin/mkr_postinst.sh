#!/bin/bash

mkdir -p -m 0777 /usr/pluto/orbiter/floorplans
ln -sf /usr/pluto/orbiter/floorplans/ /var/www/pluto-admin/
mkdir -p /usr/pluto/orbiter/users
ln -sf /usr/pluto/orbiter/floorplans/ /var/www/floorplans
ln -sf /usr/pluto/orbiter/users/ /var/www/users

mkdir -p -m 0777 /home/mediapics
ln -sf /home/mediapics /var/www/pluto-admin/mediapics

a2dissite default || /bin/true

#Index=$(grep DirectoryIndex /etc/apache/httpd.conf | sed 's/DirectoryIndex//g; s/^ *//g; s/ *$//g' | grep -v '^#')
#[ -n "$Index" ] || exit 0

#for File in $Index; do
#	[ -e "/var/www/$File" -a "$File" != "index.html" ] && exit 0
#done
#[ -e "/var/www/index.html" ] && ! diff /usr/share/apache/default-configs/apache/intro.html /var/www/index.html &>/dev/null && exit 0
#rm -f /var/www/index.html

#Redirect="<?php header('Location: support/index.php?section=home&package=0'); ?>"
#[ -e /var/www/index.php ] || echo "$Redirect" >/var/www/index.php

Site="
NameVirtualHost *
<VirtualHost *>
	ServerAdmin webmaster@localhost
	
	DocumentRoot /var/www/
	<Directory />
		Options FollowSymLinks
		AllowOverride None
	</Directory>
	<Directory /var/www/>
		Options Indexes FollowSymLinks MultiViews
		AllowOverride None
		Order allow,deny
		allow from all
		# This directive allows us to have apache2's default start page
                # in /apache2-default/, but still have / go to the right place
		RedirectMatch ^/$ /pluto-admin/
	</Directory>

	ScriptAlias /cgi-bin/ /usr/lib/cgi-bin/
	<Directory \"/usr/lib/cgi-bin\">
		AllowOverride None
		Options ExecCGI -MultiViews +SymLinksIfOwnerMatch
		Order allow,deny
		Allow from all
	</Directory>

	ErrorLog /var/log/apache2/error.log

	# Possible values include: debug, info, notice, warn, error, crit,
	# alert, emerg.
	LogLevel warn

	CustomLog /var/log/apache2/access.log combined
	ServerSignature On
	
	Alias /icons/ \"/usr/share/apache2/icons/\"
	<Directory \"/usr/share/apache2/icons\">
	    Options Indexes MultiViews
	    AllowOverride None
	    Order allow,deny
	    Allow from all
	</Directory>

    Alias /doc/ \"/usr/share/doc/\"
    <Directory \"/usr/share/doc/\">
        Options Indexes MultiViews FollowSymLinks
        AllowOverride None
        Order deny,allow
        Deny from all
        Allow from 127.0.0.0/255.0.0.0 ::1/128
    </Directory>

</VirtualHost>
"

echo "$Site" >/etc/apache2/sites-available/pluto
a2ensite pluto
/etc/init.d/apache2 reload

cmd_alias="Cmnd_Alias	PLUTO_WEBCMD = /usr/pluto/bin/SetupRemoteAccess.sh, /usr/pluto/bin/Network_Firewall.sh, /usr/pluto/bin/ListSerialPorts.sh"
sudo="www-data	ALL=(root) NOPASSWD:PLUTO_WEBCMD"

awk '!/Cmnd_Alias.*PLUTO_WEBCMD/ && !/www-data.*ALL=\(root\)/' /etc/sudoers >>/etc/sudoers.$$

echo "$cmd_alias" >>/etc/sudoers.$$
echo "$sudo" >>/etc/sudoers.$$
mv -f /etc/sudoers.$$ /etc/sudoers
chmod 440 /etc/sudoers

sed -i 's/^session.gc_maxlifetime = 1440$/session.gc_maxlifetime = 144000/' /etc/php4/apache2/php.ini
/etc/init.d/apache2 reload
