#enable MySQL networking
sed -i 's/^skip-netwoking/#skip-networking/' /etc/mysql/my.cnf
/etc/init.d/mysql restart
