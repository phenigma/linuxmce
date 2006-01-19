#!/bin/bash

#Manual init bootsplash so it can start faster
/etc/init.d/bootsplash stop 2>/dev/null

while [ 1 -eq 1 ] ;do
    # Calculate scripts started in this runlevel
    SSC=(/var/run/plutoboot/.runlevel.start.*)
    case "$SSC" in
        *\*) started=0 ;;
        *) started=${#SSC[*]}
    esac
    started=$(( $started + $progressOffset ))

    # Calculate scripts killed in this runlevel
    KSC=(/var/run/plutoboot/.runlevel.stop.*)
    case "$KSC" in
        *\*) killed=0 ;;
        *) killed=${#KSC[*]}
    esac

    # Calculate the updated progress
    progressNew=$(($started + $killed))

	# If nothing happened since last time, sleep some more
    if [ $progressNew -eq $progress -a "$progressNew" != "0" ] ;then
		sleep 0.3
		continue
    fi

    if [ -x /sbin/splash.sh ]; then
		# Update bootsplash progressbar if it exists
		# needs exported: progress, kscripts, sscripts and RUNLEVEL
        export RUNLEVEL=$runlevel
		while [ "$(( progressNew - $progress ))" != "0" ] ;do
			# We do this step by step so the progress will be smooth
			progress=$(( $progress + 1 ))
    	    /sbin/splash.sh "dummy"
		done
    else
		# If the bootsplash is not installed
		progress=$progressNew
    fi
    
    # Calculate a procent
    if [ $progress -gt 0 ] ;then
		percent=$(( $progress * 100 / ($kscripts + $sscripts) ))
    else
		percent=0
    fi

    
    # Display texts
    clear
    echo "[1mStarting Runlevel[0m: [1;32m$runlevel[0m ($percent%)"
    echo 
    echo "[1mKill Scripts [0m: $killed of $kscripts"
    echo "[1mStart Scripts[0m: $started of $sscripts"
    echo 

	# Sleep a little so we don't waste disk i/o for nothing
    sleep 0.3
done;
