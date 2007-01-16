#!/bin/bash

function DisplayUsage() {
	echo "Usage: RestartX.sh <orbiterID> <computerIP>"
}

if [[ "$#" != "2" ]] ;then
	DisplayUsage
	exit 1	
fi

orbiterID=$1
computerIP=$2

/usr/pluto/bin/LaunchRemoteCmd.sh "$computerIP" "
	. /usr/pluto/bin/Utils.sh;
#<-mkr_b_via_b->
exit
#<-mkr_b_via_e->

	bash -x /usr/pluto/bin/Xconfigure.sh --keep-resolution --update-video-driver | tee /var/log/pluto/Xconfigure.log
	EXT=
	#<-mkr_b_ubuntu_b->
	EXT=".pluto"
	#<-mkr_b_ubuntu_e->	
	cp /etc/X11/xorg.conf.pluto{"$EXT",.test}
	#kill $(ps ax|grep 'X :0 -ignoreABI -ac -allowMouseOpenFail vt7'|egrep -v 'grep|SCREEN'|awk '{print $1}')
	/usr/pluto/bin/Xres_config_end.sh y" &
