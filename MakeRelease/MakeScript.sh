O1="UPDATE Version SET VersionName='$(date +%g%m%d%H)' WHERE PK_Version=1;"
echo $O1 | mysql pluto_main
rm -rf /home/MakeRelease
rm -rf /var/www/download/deb_sarge /var/www/download/source /var/www/download/win2000xp
mkdir -p /home/MakeRelease
cd /home/MakeRelease
svn co svn://localhost/pluto2/trunk/. | tee /home/MakeRelease/svn.log
MakeRelease -o 1 -r 2,9,10,11 -m 1 -s /home/MakeRelease/trunk -n / -v 1 | tee /home/MakeRelease/MakeRelease.log
