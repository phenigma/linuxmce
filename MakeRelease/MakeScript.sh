version=3
O1="UPDATE Version SET VersionName='$(date +%g%m%d%H)' WHERE PK_Version=$version;"
echo $O1 | mysql pluto_main
rm -rf /home/MakeRelease
rm -rf /var/www/download/deb_sarge /var/www/download/source /var/www/download/win2000xp
mkdir -p /home/MakeRelease
cd /home/MakeRelease
svn co svn://localhost/pluto2/trunk/. | tee /home/MakeRelease/svn.log
MakeRelease -o 1 -r 2,9,11 -m 1 -s /home/MakeRelease/trunk -n / -v 1 | tee /home/MakeRelease/MakeRelease.log

#the following is only if it's a real build -- not version 1
# todo -- check the version
cd /var/www
rm debian.tar.gz
tar zcvf /var/www/debian.tar.gz /var/www/debian/dists/20dev/main/binary-i386/*
scp debian.tar.gz problems@69.25.176.44:~/
rm download.tar.gz
tar zcvf /var/www/download.tar.gz /var/www/download/*
scp download.tar.gz plutoadmin@69.25.176.44:~/
read

