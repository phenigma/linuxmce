#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

Parameter="$1"; shift

if [[ "$Parameter" != "start" && "$Parameter" != "stop" ]]; then
	echo "Usage: $0 start|stop"
	exit 1
fi

setterm -blank >/dev/console             # disable console blanking
chmod 777 /etc/pluto.conf                # ensure access rights on pluto.conf
/usr/pluto/bin/Report_Machine_Status.sh
rm /var/log/pluto/running.pids
rm -rf /tmp/*                            # I doubt that this is safe to do here (mee too :)
chmod 777 /var/log/pluto
rm -f /dev/ttyS_*                        # remove all ttyS_* (created by gc100s) entries from /dev
mkdir -p /usr/pluto/locks                # clean up locks
rm -f /usr/pluto/locks/*                 # clean up locks

# assure some settings
Q="SELECT FK_Installation FROM Device WHERE PK_Device='$PK_Device'"
R="$(RunSQL "$Q")"
ConfSet PK_Installation "$R"
Q="SELECT PK_Users FROM Users LIMIT 1"
R="$(RunSQL "$Q")"
ConfSet PK_Users "$R"

# remove problematic diskless services (can't poweroff if these get stopped)
# TODO: don't do this on Core (doesn't hurt though)
if [[ "$Parameter" == "stop" ]]; then
	rm -f /etc/rc{0,6}.d/S*{umountnfs.sh,portmap,networking}
fi

# If this is the first boot then we need to generate and run pluto rc scripts
if [[ "$Parameter" == "start" ]]; then
	ConfGet "FirstBoot"
	if [[ "$FirstBoot" != "false" ]] ;then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating and running Pluto rc startup scripts for the first time"
		/usr/pluto/bin/generateRcScripts.sh
		for i in /etc/rc2.d/S22-*-Pluto_* ;do
			[[ ! -f $i ]] && continue
			$i start
		done	
		ConfSet "FirstBoot" "false"
	fi	    					
fi

# These is left here only for logging
if [[ -e /usr/pluto/install/.notdone ]]; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "It appears the installation was not successful. Pluto's startup scripts are disabled. To enable them, complete the installation process, or manually remove the file /usr/pluto/install/.notdone"
	exit 1
fi

# Someone said this fixed his NFS problems; I'm putting it here for both Core and MDs
# Canceled because of PPPoE MTU issues
#echo 1 >/proc/sys/net/ipv4/ip_no_pmtu_disc

# apt-proxy was replaced in 2.0.0.12 with the polipo+frox combination
# translating all apt-proxy references in sources.list
if grep -qF dcerouter:9999 /etc/apt/sources.list 2>/dev/null; then
	sed -i 's,http://dcerouter:9999/,,g' /etc/apt/sources.list
fi

# Errors are too verbose by default
dmesg -n 1
