#!/bin/sh

FIRMWARE_URL="$1";
FIRMWARE_MD5="$2";

log() {
	logger -s -t 'fiire-upgrade' $*
}

# Download the requested firmware
log "Downloading firmware $FIRMWARE_URL"
cd /tmp
wget -O /tmp/firmware.img "$FIRMWARE_URL"

# Check the download checksum
if [[ `md5sum /tmp/firmware.img | cut -d' ' -f1` != "$FIRMWARE_MD5" ]] ;then
	log "Download failed. Checksum does not match."
	exit 1
else
	log "Download finished."
fi

# Stop services
log "Closing down services."
/etc/init.d/httpd stop
/etc/init.d/Pluto stop

# Backup configuration
log "Creating a backup of the config files."
tar zcf /tmp/backup.tar.gz /etc/pluto /etc/config

# Flash new image
log "Started flashing new image."
cd /tmp
mtd write /tmp/firmware.img linux
rm -f /tmp/firmware.img

# Restore config files
cd /
tar zxvf /tmp/backup.tar.gz

# Rebooting
reboot
