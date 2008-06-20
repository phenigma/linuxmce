#!/bin/sh

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
