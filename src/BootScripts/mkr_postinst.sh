update-rc.d Startup_Scripts.sh defaults 99 10

# Changed from 2.0.0.10 to 2.0.0.11: diskless filesystems were moved to /home
if [ ! -L /usr/pluto/diskless -a -d /usr/pluto/diskless ]; then
	mv /usr/pluto/diskless /home
fi
ln -sf /home/diskless /usr/pluto/diskless
