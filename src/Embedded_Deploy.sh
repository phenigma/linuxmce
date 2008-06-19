#!/bin/bash
D=embedded_deploy
rm -rf $D
mkdir -p $D

# Bin
mkdir -p $D/usr/bin
cp bin/* $D/usr/bin
cp -a Embedded_Boot/* $D

# Lib
mkdir -p $D/usr/lib
cp lib/* $D/usr/lib


# Web
mkdir -p $D/www/
cp -a ../web/cisco/ $D/www
mv $D/www/cisco/cgi-bin/ $D/www

sed -ri '/^var deployed=/ s/false/true/' $D/www/cisco/js/scripts.js
rm -f $D/www/cisco/{*.json,data.txt}
sed -ri '/^<!--deploy:remove-->/d; /^<!--deploy:enable-->/ s/^<!--deploy:enable--><!--(.*)-->/\1/g' $D/www/cisco/index.html

# Cleanup
find $D -type d -name .svn -exec rm -rf '{}' ';' -prune

echo "Please populate the /etc/pluto directory"
