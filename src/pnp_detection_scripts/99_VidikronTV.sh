#!/bin/bash

echo "Vidikron script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 19200 -t "\r\n\s5000m[S3E0001\s1000m[S3E0000\s500m" -i 1 -s "]0000"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Vidikron"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Vidikron"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1900
fi
