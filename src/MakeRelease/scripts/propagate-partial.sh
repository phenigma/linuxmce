mv *.deb /var/www/plutohome/debian/dists/20dev/main/binary-i386
rm *.changes
cd /var/www/plutohome/debian/dists/20dev/main/binary-i386
../update-repository

