#!/bin/bash
echo 'CREATE DATABASE IF NOT EXISTS `asterisk`;' | mysql

# TODO: treat upgrades
mysql asterisk </usr/pluto/install/asterisk.sql


# update res_mysql.conf
: >/etc/asterisk/res_mysql.conf

TemplateMysqlConf='[general]
dbhost = localhost
dbname = asterisk
dbuser = root
dbpass = 
dbport = 3306
dbsock = /var/run/mysqld/mysqld.sock
'

eval "echo \"$TemplateMysqlConf\"" >> /etc/asterisk/res_mysql.conf

# update extconfig.conf
: >/etc/asterisk/extconfig.conf

echo "[settings]
sipfriends => mysql,asterisk,sip_buddies
realtime_ext => mysql,asterisk,extensions_table" >> /etc/asterisk/extconfig.conf

# update extensions

: >/etc/asterisk/extensions.conf

echo "[trusted]
switch => Realtime/trusted@realtime_ext
[outgoing-extern]
switch => Realtime/outgoing-extern@realtime_ext
[outgoing-local]
switch => Realtime/outgoing-local@realtime_ext
[outgoing-extern-selectline]
switch => Realtime/outgoing-extern-selectline@realtime_ext
[outgoing-place-call]
switch => Realtime/outgoing-place-call@realtime_ext" >> /etc/asterisk/extensions.conf

# update manager.conf
: >/etc/asterisk/manager.conf

echo "[general]
enabled = yes
port = 5038
bindaddr = 0.0.0.0

[admin]
secret = adminsecret
read = system,call,log,verbose,command,agent,user
write = system,call,log,verbose,command,agent,user" >> /etc/asterisk/manager.conf




