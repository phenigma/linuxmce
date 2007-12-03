
if ! grep -q '^Name: shared/accepted-sun-dlj-v1-1' /var/cache/debconf/config.dat ;then
	echo 'Name: shared/accepted-sun-dlj-v1-1' >> /var/cache/debconf/config.dat
	echo 'Template: shared/accepted-sun-dlj-v1-1' >> /var/cache/debconf/config.dat
	echo 'Owners: sun-java6-bin, sun-java6-jre' >>  /var/cache/debconf/config.dat
fi

. /usr/share/debconf/confmodule

db_version 2.0
db_capb backup

## Set deb-frontend to noninteractive
db_set "debconf/frontend" "Noninteractive"
db_fset "debconf/frontend" seen true
db_set "debconf/priority" "critical"
db_fset "debconf/priority" seen true

db_set "shared/accepted-sun-dlj-v1-1" 'true'
db_fset "shared/present-sun-dlj-v1-1" seen true
db_fset "shared/error-sun-dlj-v1-1" seen true
db_fset "shared/accepted-sun-dlj-v1-1" seen true

