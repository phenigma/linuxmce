#!/bin/bash

HELPER="./dvdcssHelper"
#HELPER="/usr/pluto/bin/dvdcssHelper"

LOG="/tmp/dvdcssHelper.$$"

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
    echo "Unknown switch"
esac

