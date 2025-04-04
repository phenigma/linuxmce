#!/bin/bash
source setEnv.sh

#a check for the following would be nice
#a package listing these as dependencies would be better
#dreams
sudo apt-get install build-essential git autoconf automake libtool

#check for this somehow? makes life easier.
ssh-copy-id $TARGET_USER@$TARGET_IP

echo "Setting up Raspberry Pi working directory"


if [ -d "rpi-tools" ];
then
echo "Rpi tools Directory in place"
else
echo "Making rpi-tools directory"
mkdir rpi-tools
cd rpi-tools
echo "cloning initial tools from git"
git clone https://github.com/raspberrypi/tools.git
echo "cloning additional tools"
git clone git://gitorious.org/cross-compile-tools/cross-compile-tools.git
fi

clear

#echo "Installing remote things, please be prepared to enter password"
#ssh $TARGET_USER@$TARGET_IP "sudo apt-get build-dep qt4-x11"
#ssh $TARGET_USER@$TARGET_IP "sudo apt-get build-dep libqt5gui5"
#ssh $TARGET_USER@$TARGET_IP "sudo apt-get install libudev-dev libinput-dev libts-dev libxcb-xinerama0-dev"

#ssh $TARGET_USER@$TARGET_IP "sudo mkdir /usr/local/qt5pi"
#ssh $TARGET_USER@$TARGET_IP "sudo chown $TARGET_USER:$TARGET_USER /usr/local/qt5pi"

if [ -e "rpi-tools/sysroot-relativelinks.py" ];
then
echo "Have symlink file, will run symlinks fix script"
else
echo "Getting symlink fix script and run symlinks fix script"
wget https://raw.githubusercontent.com/riscv/riscv-poky/master/scripts/sysroot-relativelinks.py
chmod +x sysroot-relativelinks.py
mv sysroot-relativelinks.py rpi-tools
fi

if [ -d "rpi-tools/sysroot" ];
then
echo "Sysroot in place connecting to $TARGET_USER@$TARGET_IP:"
cd "rpi-tools"
rsync -avz $TARGET_USER@$TARGET_IP:/lib sysroot
rsync -avz $TARGET_USER@$TARGET_IP:/usr/include sysroot/usr
rsync -avz $TARGET_USER@$TARGET_IP:/usr/lib sysroot/usr
rsync -avz $TARGET_USER@$TARGET_IP:/opt/vc sysroot/opt
./sysroot-relativelinks.py sysroot
else
echo "Creating sysroot"
cd "rpi-tools"
mkdir sysroot sysroot/usr sysroot/opt
rsync -avz $TARGET_USER@$TARGET_IP:/lib sysroot
rsync -avz $TARGET_USER@$TARGET_IP:/usr/include sysroot/usr
rsync -avz $TARGET_USER@$TARGET_IP:/usr/lib sysroot/usr
rsync -avz $TARGET_USER@$TARGET_IP:/opt/vc sysroot/opt
./sysroot-relativelinks.py sysroot
fi
clear

#echo "Please login and run /tmp/pi-localSetup.sh, the exit the pi"
#echo "command: sudo /tmp/./pi-localSetup.sh"
#scp  $STARTPATH/pi-localSetup.sh $TARGET_USER@$TARGET_IP:/tmp/
#ssh  $TARGET_USER@$TARGET_IP



#if [ -d "$ROOT_FS_MOUNT" ];
#then
#echo "Mount point is already setup, ensure your device can be contacted"
#else
#echo "Setting up mount point, you will need sudo access"
#sudo mkdir $ROOT_FS_MOUNT
#sudo chmod 755 $ROOT_FS_MOUNT
#fi

#sudo apt-get install libfontconfig1-dev libdbus-1-dev libfreetype6-dev libudev-dev libicu-dev libsqlite3-dev libxslt1-dev libssl-dev libasound2-dev libavcodec-dev #libavformat-dev libswscale-dev libraspberrypi-dev libpulse-dev libx11-dev libglib2.0-dev libcups2-dev freetds-dev libsqlite0-dev libpq-dev libiodbc2-dev #libmysqlclient-dev firebird-dev libpng12-dev libjpeg62-dev libgst-dev libxext-dev libxcb1 libxcb1-dev libx11-xcb1 libx11-xcb-dev libxcb-keysyms1 libxcb-keysyms1-#dev libxcb-image0 libxcb-image0-dev libxcb-shm0 libxcb-shm0-dev libxcb-icccm4 libxcb-icccm4-dev libxcb-sync0 libxcb-sync0-dev libxcb-render-util0 libxcb-render-#util0-dev libxcb-xfixes0-dev libxrender-dev libxcb-shape0-dev libxcb-randr0-dev libxcb-glx0-dev libxi-dev libdrm-dev

#sudo apt-get install debhelper dh-apparmor freetds-common freetds-dev gir1.2-atspi-2.0 gir1.2-gst-plugins-base-0.10 gir1.2-gstreamer-0.10 gir1.2-gtk-2.0 icu-devtools libatk1.0-dev libatspi2.0-dev libcairo-script-interpreter2 libcairo2-dev libct4 libcups2-dev libdbus-glib-1-dev libgbm-dev libgdk-pixbuf2.0-dev libgstreamer-plugins-base0.10-dev libgstreamer0.10-dev libgtk2.0-dev libicu-dev libjpeg-dev libjpeg-turbo8-dev libjpeg8-dev libodbc1 libopenvg1-mesa-dev libpango1.0-dev libpixman-1-dev libpq-dev libsqlite3-dev libsybdb5 libudev-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-render-util0-dev libxcb-xkb-dev libxcomposite-dev libxcursor-dev libxft-dev libxi-dev libxinerama-dev libxkbcommon-dev  libxkbcommon-x11-dev libxml2-dev libxrandr-dev libxrender-dev libxtst-dev po-debconf unixodbc unixodbc-dev x11proto-composite-dev x11proto-randr-dev x11proto-record-dev x11proto-render-dev x11proto-xinerama-dev xvfb


exit