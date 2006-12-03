#!/bin/bash

echo "$(date) $0-$1-$2-$3-$4" >> /var/log/pluto/donerecording.log
/usr/pluto/bin/MessageSend dcerouter 0 -1001 2 69 29 "$1" 57 "$2" 35 "$3/$4"

