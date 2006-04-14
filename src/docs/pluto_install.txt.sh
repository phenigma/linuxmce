#!/bin/sh
# Author    : C Remus
# Change by : ...
#
# version     : alpha
# description : only notes now
cat $0
exit 0

# install pluto devel required libs (as root user)
su
aptitude install xlibs-static-dev libpcre3-dev libxp-dev pm-dev libfontconfig1-dev libxrandr-dev libsdl-sound1.2-dev libsdl-sge-dev libxpm-dev libxi-dev libsvga1-dev libcupsys2-dev libgnutls11-dev libgpg-error-dev kdelibs4-dev libaudiofile-dev aalib1-dev libsm-dev kdebase-dev libkonq4-dev render-dev libgcrypt11-dev libxt-dev libsdl-stretch-dev liblcms1-dev libxcursor-dev libaudio-dev libxtrap-dev libglib1.2-dev libdmalloc4-dev libxtst-dev libgtk1.2-dev libexpat1-dev libopencdk8-dev libstlport4.6-dev libstdc++6-dev x-dev libxmu-dev libgtk2.0-dev libxine-dev libvorbis-dev xlibmesa-gl-dev libsdl-ttf2.0-dev libxext-dev libmng-dev libogg-dev libxrender-dev libsdl1.2-dev manpages-dev libncurses5-dev libtasn1-2-dev libtiff4-dev libstdc++5-3.3-dev libsdl-net1.2-dev libasound2-dev libuclibc-dev libxv-dev libarts1-dev libjpeg62-dev libsdl-gfx1.2-dev libpng12-dev libc6-dev slang1-dev libidn11-dev libjack0.80.0-dev libmad0-dev libsmpeg-dev dpkg-dev libsdl-mixer1.2-dev libxmuu-dev libfreetype6-dev libart-2.0-dev libartsc0-dev xlibs-dev libsdl-image1.2-dev manpages-posix-dev libttf-dev libglib2.0-dev qt3-apps-dev libattr1-dev libesd0-dev libmysqlclient12-dev libid3-3.8.3-dev libsdl-console-dev libpango1.0-dev libhttpfetcher-dev zlib1g-dev libx11-dev autotools-dev libice-dev libqt3-mt-dev libssl-dev xlibmesa-glu-dev libatk1.0-devlibxft-dev libfam-dev
exit

# base directory for sources
# // can be any name or location
# reffered as PLUTO_SRC_BASE from now on
# PLUTO_SRC_BASE is just a name chosed for this installation
# PLUTO_SRC_BASE must contain the full path
export PLUTO_SRC_BASE=~/pluto
mkdir $PLUTO_SRC_BASE

# ACCOUNT_DEVEL_SERVER : valid account on the development server
# ACCOUNT_HYBRID_MACHINE : valid account on a installed hybrid-test-machine
# names chosed for this installation
# change them as appropiate
export ACCOUNT_DEVEL_SERVER=xxxx@10.0.0.150
export ACCOUNT_HYBRID_MACHINE=root@10.0.0.89

# get sources from svn
cd $PLUTO_SRC_BASE
svn co http://10.0.0.170/pluto/trunk/libs/
svn co http://10.0.0.170/pluto/trunk/src/

# pluto compatibility directories
su
mkdir /usr/pluto/
cd /usr/pluto/
ln -s $PLUTO_SRC_BASE/src/bin
ln -s $PLUTO_SRC_BASE/src/lib
echo -en '/usr/pluto/bin/\n/usr/pluto/lib/\n' >> /etc/ld.so.conf
ldconfig
mkdir -p /home/public/data/
scp -pr $ACCOUNT_DEVEL_SERVER:/home/samba/repositories/pluto/20dev/non-free/binary-i386/test-media/home/public/data/* /home/public/data/
mkdir -pv /usr/pluto/orbiter/skins
scp -pr $ACCOUNT_HYBRID_MACHINE:/usr/pluto/orbiter/skins /usr/pluto/orbiter/
mkdir -pv /usr/share/fonts/truetype/msttcorefonts
scp -pr $ACCOUNT_HYBRID_MACHINE:/usr/share/fonts/truetype/msttcorefonts /usr/share/fonts/truetype/
cd /usr/pluto/
chown $LOGNAME.$LOGNAME -R *
exit

echo export LD_LIBRARY_PATH=/usr/pluto/lib:/usr/pluto/bin >> ~/.bashrc

# all_new_deps: pluto_main Xine_Plugin/so Xine_Player/bin
# make all
# make -C SerializeClass

# --------------------------------------------------
# local hybrid-like machine

# get sources from svn
cd $PLUTO_SRC_BASE
svn co http://10.0.0.170/pluto/trunk/web/

# login as root
su
aptitude install mysql-client mysql-server apache2 libapache2-mod-php4 php4-mysql
cd /var/www
ln -sv $PLUTO_SRC_BASE/web/pluto-admin/
scp -pr $ACCOUNT_HYBRID_MACHINE:/var/www/globalconfig .
# ! edit /etc/php4/apache2/php.ini
# ! uncomment line: ;extension=mysql.so
/etc/init.d/apache2 restart
exit

# save remote database
ssh $ACCOUNT_HYBRID_MACHINE
mysqldump --opt mysql > /tmp/pluto_mysql.sql
mysqldump --opt --databases pluto_main pluto_media pluto_myth pluto_security pluto_telecom asterisk asteriskcdrdb mythconverg > /tmp/pluto_db.sql
exit

# get and import db
cd /tmp/
scp -p $ACCOUNT_HYBRID_MACHINE:/tmp/pluto_*.sql .
mysql -u root < pluto_db.sql
mysql -u root mysql < pluto_mysql.sql

# eventually
su
/etc/init.d/mysql restart
exit

# copy already compiled sources and libs in the right places
scp -p $ACCOUNT_DEVEL_SERVER:/home/newbins.tar.bz2 $ACCOUNT_DEVEL_SERVER:/home/newso.tar.bz2 /tmp/
cd $PLUTO_SRC_BASE/bin
tar -jxvf newbins.tar.bz2
cd $PLUTO_SRC_BASE/lib
tar -jxvf newso.tar.bz2

# getting the correct id
#
# open in browser http://localhost/pluto-admin/
# login
# click on the left frame, at the bottom: 'Show devices tree'
# click on the left frame: CORE -> The core/hybrid -> OnScreen Orbiter
# look in the right frame, at top-left: Device Info #18
# 18 is the id you need, it may be other number on your machine

# generate screens
# replace 18 in the line below with the id from the admin-page
/usr/pluto/bin/OrbiterGen -h localhost -d 18 -g /usr/pluto/orbiter/skins -f /usr/share/fonts/truetype/msttcorefonts -o /usr/pluto/orbiter

# DONE

# END
