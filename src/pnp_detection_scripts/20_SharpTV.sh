#!/bin/bash

echo "Sharp TV Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "POWR1   \r\s500mPOWR0   \r" -i 1 -s "OK\r"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Sharp TV"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Sharp TV"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1856
fi
