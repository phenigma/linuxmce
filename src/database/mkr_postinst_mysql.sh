#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

#enable MySQL networking
MyCnf=/etc/mysql/my.cnf
DefTableType=innodb
if [ ! -e /etc/mysql/my.cnf.pbackup ] ;then
	cp "$MyCnf" "$MyCnf".pbackup
fi
sed -i "s/^skip-networking/#skip-networking/; s/^skip-innodb/#skip-innodb/; s/^default-table-type=.*$/default-table-type=$DefTableType/" "$MyCnf"
sed -i 's/^bind-address.*$/bind-address=0.0.0.0/; s/\(^log.*=.*$\)/#\1/g' "$MyCnf"
sed -i 's/^expire_logs_days/#expire_logs_days/g' "$MyCnf"
grep -q '^default-table-type=' "$MyCnf" || echo "default-table-type=$DefTableType" >>"$MyCnf"
echo "GRANT ALL PRIVILEGES ON pluto_main.* to 'root'@'127.0.0.1';" | mysql
echo "FLUSH PRIVILEGES;" | mysql
invoke-rc.d mysql restart
