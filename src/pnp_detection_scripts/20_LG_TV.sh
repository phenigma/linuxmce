#!/bin/bash

echo "LG TV Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "ka 01 01\r\n" -s "a 01 " -i 8

if [[ "$?" -ne 0 ]]; then
echo "It's not a LG TV"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is an LG TV"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1915
fi
