#! /bin/sh
#
# Coldplug hardware replacement for hotplug
#
# Written by Razvan Gavril <razvan.g@plutohome.com>

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
LSHWD=/usr/bin/lshwd
NAME=lshwd
DESC="coldplug hardware detection"

test -x /usr/bin/lshwd || exit 0

set -e

case "$1" in
    start|reload|restart)
        echo -n "Starting $DESC ...";
        $LSHWD -a
    ;;
    stat)
        /usr/bin/lshwd -cc
    ;;
    *)
        echo "Usage: $0 {start|reload|restart|stat}"
        exit 1
    ;;
esac

exit 0
