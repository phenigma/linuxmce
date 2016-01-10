#
# This file is called by /etc/init.d/vdr
#

#
# Defaults - don't touch, edit options for the VDR daemon in
# /etc/default/vdr !!!
#

# Config-Directory
CFG_DIR="/var/lib/vdr"

# Plugin-Directory
PLUGIN_DIR="/usr/lib/vdr/plugins"

# Plugin Config-Directory
PLUGIN_CFG_DIR="/etc/vdr/plugins"

# Plugin prefix
PLUGIN_PREFIX="libvdr-"

# Command-Hooks Directory
CMDHOOKSDIR="/usr/share/vdr/command-hooks"

# Commmand executed on start, stop and editing of a recording
REC_CMD=/usr/lib/vdr/vdr-recordingaction

# Commmand executed by vdr to shutdown the system
SHUTDOWNCMD="/etc/init.d/vdr stop ; sleep 1 ; /sbin/shutdown -h now"

# EPG data file
EPG_FILE=/var/cache/vdr/epg.data

# Default port for SVDRP
SVDRP_PORT=2001

# Enable / Disable vdr daemon
ENABLED=0

# Enable / Disable automatic shutdown
ENABLE_SHUTDOWN=0

# Allow VDR to use NPTL (if available) - disabled by default
# (This has no effect on AMD64 machines.)
NONPTL=1


#
# Read configured options from /etc/default/vdr
#
 
test -f /etc/default/vdr && . /etc/default/vdr
