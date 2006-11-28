#!/bin/bash

function readConf {
	[[ -r /etc/pluto.conf ]] || return 1

	var=$1

	val=$(grep "${var} *=" /etc/pluto.conf)
	val=${val#*=*}
	val=${val# *}

	if [[ "$val" != "" ]] ;then
		eval "$var=\"$val\""
	else
		return 1
	fi
}

test -x /usr/pluto/bin/DCERouter || exit 0

readConf MySqlHost		|| MySqlHost="localhost"
readConf MySqlUser		|| MySqlUser="root"
readConf MySqlPassword	|| MySqlPassword=""
readConf MySqlDBName	|| MySqlDBName="pluto_main"
readConf MySqlPort		|| MySqlPort="3306"

. /lib/lsb/init-functions

case "$1" in
	start)
		log_daemon_msg "Starting DCE Router: "

		start-stop-plutodaemon.sh -S -n "DCERouter" --  /usr/pluto/bin/DCERouter -h "$MySqlHost" -u "$MySqlUser" -p "$MySqlPassword" -D "$MySqlDBName" -p "$MySqlPort"
		
		if [[ $? != "0" ]] ;then
			log_end_msg 1
			exit 1
		fi

		log_end_msg 0
		;;
	stop)
		log_daemon_msg "Stoping DCE Router: "
		start-stop-plutodaemon.sh -K -n "DCERouter"

		log_end_msg 0
		;;
esac

exit 0
