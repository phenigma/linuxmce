#!/bin/bash

#HELPER="./dvdcssHelper"
HELPER="/usr/pluto/bin/dvdcssHelper"

LOG="/tmp/dvdcssHelper.$$"

if [ ! -f "/usr/lib/libdvdcss.so.2" ]; then
	echo "ERROR:No DVDCSS support"
	exit 1
fi

case "$1" in
  -f)
   $HELPER $1 "$2" 1>$LOG 2>&1
   ERROR=`grep -m 1 "^Error" $LOG | sed "s/Error: //"`
   if [ -n "$ERROR" ]; then
       echo "ERROR:$ERROR"
       exit 1
   fi

   CACHE_DIR=`grep "CSS key cache" $LOG | sed "s/libdvdcss debug: using CSS key cache dir: //" | sed "s/\/\$//"`
   echo "CACHE_DIR:$CACHE_DIR"
  ;;

  -d)
    $HELPER $1 "$2" 1>$LOG 2>&1
    ERROR=`grep -m 1 "^Error" $LOG | sed "s/Error: //"`
    if [ -n "$ERROR" ]; then
	echo "ERROR:$ERROR"
	exit 1
    fi

    KEY_ERROR_COUNT=`grep "Error cracking CSS key for" $LOG | wc -l`
    echo "KEY_ERROR_COUNT:$KEY_ERROR_COUNT"
  ;;

  *)
    echo "Usage: dvdcssHelper (-f|-d) /path/to/dvd"
    echo -e "-f\tprint DVDCSS keys cache folder for DVD"
    echo -e "-d\tdump  DVDCSS keys for DVD to the cache folder"
esac

