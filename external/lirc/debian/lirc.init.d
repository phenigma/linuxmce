#! /bin/sh
#
#

load_modules ()
{
	local MODULES_MISSING=false

	for mod in $*
	do
		modprobe -k $mod 2> /dev/null || MODULES_MISSING=true
	done

	if $MODULES_MISSING; then
		echo "#####################################################"
		echo "## I couldn't load the required kernel modules     ##"
		echo "## You should install lirc-modules-source to build ##"
		echo "## kernel support for your hardware.               ##"
		echo "#####################################################"
		echo "## If this message is not appropriate you may set  ##"
		echo "## LOAD_MODULES=false in /etc/lirc/hardware.conf   ##"
		echo "#####################################################"
		START_LIRCMD=false
		START_LIRCD=false
	fi

	if test -x /sbin/udevsettle
	then
		if ! /sbin/udevsettle; then
		  echo "timeout waiting for devices to be ready"
		fi
	fi
}

build_args ()
{
	local ARGS="$*"
	
    ## Try to find an lirc device.
    ## udev uses /dev/lirc0
    ## static dev uses /dev/lirc
    ## devfs uses /dev/lirc/0
    if [ -z "$DEVICE" ]; then
    	for dev in /dev/lirc0 /dev/lirc /dev/lirc/0; do
	    	if [ -c $dev ]; then
				DEVICE="$dev"
				break
			fi
		done
	fi
	
	if [ -n "$DEVICE" ] && [ "$DEVICE" != "none" ]; then
		ARGS="--device=$DEVICE $ARGS"
	fi
	if [ -n "$DRIVER" ] && [ "$DRIVER" != "none" ]; then
		ARGS="--driver=$DRIVER $ARGS"
	fi
	echo $ARGS
}

test -f /usr/sbin/lircd || exit 0
test -f /usr/sbin/lircmd || exit 0
#test -f /etc/lirc/lircd.conf || exit 0
#test -f /etc/lirc/lircmd.conf || exit 0

START_LIRCMD=true
START_LIRCD=true

if [ ! -f /etc/lirc/lircd.conf ] \
	|| grep -q "^#UNCONFIGURED"  /etc/lirc/lircd.conf;then
	if [ "$1" = "start" ]; then
          echo "##################################################"
          echo "## LIRC IS NOT CONFIGURED                       ##"
          echo "##                                              ##"
          echo "## read /usr/share/doc/lirc/html/configure.html ##"
          echo "##################################################"
	fi
	START_LIRCD=false
	START_LIRCMD=false
fi
if [ ! -f /etc/lirc/lircmd.conf ] \
	|| grep -q "^#UNCONFIGURED" /etc/lirc/lircmd.conf;then
	START_LIRCMD=false
fi

if [ -f /etc/lirc/hardware.conf ];then
	. /etc/lirc/hardware.conf
fi


case "$1" in
  start)
    if [ "$LOAD_MODULES" = "true" ] && [ "$START_LIRCD" = "true" ]; then
	load_modules $MODULES
    fi
    echo -n "Starting lirc daemon:"
    if $START_LIRCD; then
      echo -n " lircd"
      LIRCD_ARGS=`build_args $LIRCD_ARGS`
      start-stop-daemon --start --quiet --exec /usr/sbin/lircd -- $LIRCD_ARGS \
		< /dev/null
    fi
    if $START_LIRCMD; then
      echo -n " lircmd"
      start-stop-daemon --start --quiet --exec /usr/sbin/lircmd \
      		< /dev/null
    fi
    echo "."
    ;;
  stop)
    echo -n "Stopping lirc daemon:"
    echo -n " lircmd"
    start-stop-daemon --stop --quiet --exec /usr/sbin/lircmd
    echo -n " lircd"
    start-stop-daemon --stop --quiet --exec /usr/sbin/lircd
    echo "."
    ;;
  reload|force-reload)
    if $START_LIRCD; then
      start-stop-daemon --stop --quiet --signal 1 --exec /usr/sbin/lircd
    fi
    if $START_LIRCMD; then
      start-stop-daemon --stop --quiet --signal 1 --exec /usr/sbin/lircmd
    fi
    ;;
  restart)
    $0 stop
    $0 start
    ;;
  *)
    echo "Usage: /etc/init.d/lircd {start|stop|reload|restart|force-reload}"
    exit 1
esac

exit 0
