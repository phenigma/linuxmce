#!/bin/bash

#enable MySQL networking
MyCnf=/etc/mysql/my.cnf
DefTableType=innodb
sed -i "s/^skip-networking/#skip-networking/; s/^skip-innodb/#skip-innodb/; s/^default-table-type=.*$/default-table-type=$DefTableType/" "$MyCnf"
grep -q '^default-table-type=' "$MyCnf" || echo "default-table-type=$DefTableType" >>"$MyCnf"
echo "GRANT ALL PRIVILEGES ON pluto_main.* to 'root'@'127.0.0.1';" | mysql
echo "FLUSH PRIVILEGES;" | mysql
