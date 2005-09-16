#!/bin/bash

rm -rf /var/www/download.bak
mv /var/www/download /var/www/download.bak

mkdir /var/www/download
cd /var/www/download
if ! tar -xzvf /home/uploads/download.tar.gz; then
	echo "Untar failed"
	exit 1
fi

current_version="$(</var/www/download/current_version)"
echo Is this version right $current_version
read
mv /var/www/download/installation-cd.$version_name.iso /var/www
mv /var/www/download/installation-cd.$version_name.md5 /var/www
ln -sf /var/www/static_download/docs/ /var/www/download/docs
ln -sf /var/www/static_download/installers/ /var/www/download/installers
ln -sf /var/www/static_download/winlib-src/ /var/www/download/winlib-src
ln -sf /var/www/winlib /var/www/download/winlib
ln -sf /var/www/winnetdlls /var/www/winnetdlls
ln -sf /var/www/contrib/ /var/www/download/debian/contrib

rm /home/httpd/vhosts/plutohome.com/anon_ftp/pub/pldebsrg.iso
cp /var/www/download/cds/pldebsrg.iso /home/httpd/vhosts/plutohome.com/anon_ftp/pub/pldebsrg.iso

ls /home/uploads -ltr
echo "ready to get replacements"
read
bash -x /home/uploads/replacements.patch.sh

cd /var/www
cdmd5="$(</var/www/installation-cd.$version_name.md5)"
sed "s/****KICK_START_NAME****/installation-cd.$version_name.iso/g" /var/www/plutohome-com/client/step8a.php > /tmp/step8a.tmp
sed "s/****MD5SUM****/$cdmd5/g" /tmp/step8a.tmp /tmp/step8a.php
mv /tmp/step8a.php /var/www/plutohome-com/client/step8a.php
echo "Confirm cd version is installation-cd.$version_name.iso and md5"
cat /var/www/installation-cd.$version_name.md5
md5sum /var/www/installation-cd.$version_name.iso
read
ls -l
echo "remove old iso images"
read

