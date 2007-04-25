#!/bin/bash

. /tmp/mce_wizard_data.sh

rm -rf /tmp/mce_installer_error
InstallerLogFile="/var/log/mce-installer-$(date +%s).log"
echo "--- start mce wizard data ---" >> $InstallerLogFile
cat /tmp/mce_wizard_data.sh >> $InstallerLogFile
echo "--- end mce wizard data ---" >> $InstallerLogFile
exec &> >(tee $InstallerLogFile)

function ExitInstaller {
	echo 
	echo "ERROR : $*" >&2
	echo
	echo "$*" > /tmp/mce_installer_error
	exit 1
}

apt-get install -y libbluetooth2 libbluetooth2-dev mysql-common libssl-dev zlib1g-dev libaa1 libaa1-dev libasound2 libasound2-dev libncurses5-dev libsdl1.2debian-all libx11-6 libx11-dev libxext6 libxext-dev libxi6 libxi-dev libxv1 libxv-dev x-dev xlibmesa-gl-dev libglu1-xorg-dev xlibs-static-dev libfreetype6 libfreetype6-dev libsdl-ttf2.0-0 libsdl-ttf2.0-dev libjpeg62 libjpeg62-dev libpng12-0 libpng12-dev libsdl-image1.2 libtiff4 libtiff4-dev libsdl-image1.2-dev libattr1-dev libice6 libice-dev libogg0 libsm6 libsm-dev libspeex1 libvorbis0a libxmu6 libxmu-dev libxmuu1 libxmuu-dev libxp6 libxp-dev libxpm4 libxpm-dev libxrandr2 libxrandr-dev libxrender1 libxrender-dev libxt6 libxt-dev libxtrap6 libxtrap-dev libxtst6 libxtst-dev render-dev xlibs-dev libqt3-mt libqt3-headers defoma fontconfig libaudio2 libaudio-dev libexpat1 libfontconfig1 libfontconfig1-dev libglib2.0-0 libglib2.0-data liblcms1 liblcms1-dev libmng-dev libmng1 libqt3-compat-headers libqt3-mt-dev libttf2 libxcursor1 libxcursor-dev libxft2 libxft-dev pkg-config qt3-dev-tools ttf-bitstream-vera ucf libartsc0 mysql-server zlib1g gcc libopenobex1 bluez-utils apache2 ethtool screen netcat make subversion cvs mysql-client ssh cdrecord nfs-kernel-server vim samba syslinux dhcp3-server bind9 bzip2 ntpdate discover1 alsa-base less alsa-utils iproute perl unixodbc curl eject sudo whois ntp-simple libgtk2.0-0 libgtk2.0-dev libraw1394-8 libraw1394-dev libdc1394-13 libdc1394-13-dev ruby1.8-dev swig libtcltk-ruby1.8 ruby1.8 attr flac libao2 libflac7 liboggflac3 libvorbisenc2 libvorbisfile3 vorbis-tools cdparanoia libcdparanoia0 libsqlite0 libgtk1.2-common libgtk1.2 imagemagick udev libid3-3.8.3c2a libid3tag0-dev anacron bind9-host hal smbfs libwww-perl ttf-kochi-gothic libgl1-mesa-dri bc smbclient smbclient libid3-3.8.3-dev autofs libxml-simple-perl libxvmc1 nis xfwm4 xfce4-mcs-manager nbd-server nbd-client mdadm libhal-dev libdbus-glib-1-dev libxml2-dev libcdparanoia0-dev libcurl3-dev nvidia-glx libxml2
apt-get install -y msttcorefonts ivtv-utils liblame0 flashplugin-nonfree mythweb mythtv-backend mythtv-database mythtv-frontend libmyth-0.20 libmyth-0.20-dev
apt-get install -y libmysqlclient15off libsvga1 libsvga1-dev libslang1-dev xlibmesa-gl libglu1-xorg xlibs-data libsdl-sge libsdl-sge-dev libft-perl lsscsi libqt3-mt-mysql libssl0.9.7 xine-ui xmltv-util openobex-apps x-window-system-core libapache2-mod-php5 php5-curl php5-mysql atftpd php5-gd etherwake flite libsdl-gfx1.2-dev ftp-upload mozilla-firefox socat libmyodbc motion libsdl-gfx1.2-dev liblinphone1 liblinphone1-dev libosip2-3 libosip2-dev polipo frox libcrypt-smbhash-perl libdigest-md4-perl abcde cd-discid vlc asterisk-config xdialog xloadimage libdvdread3 ffmpeg libdc1394-examples xcompmgr libwxgtk2.6-0 icewm libdvdnav4 libdvdnav-dev ddclient nbtscan libhid-dev libhid0 arping libsoap-lite-perl libcddb2 libcddb2-dev linphone-common perlmagick openbsd-inetd libdvdread-dev debootstrap libclass-dbi-sqlite-perl


#ExitInstaller "You Suck !"
