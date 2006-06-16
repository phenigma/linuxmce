#!/bin/bash

echo "device $1"
echo "queue $2"
echo "path $3"
echo "name $4"


echo "Denon Detection script"
/usr/pluto/bin/TestSerialPort -p /dev/ttyUSB0 -P N81 -b 9600 -t "PWSTANDBY\r\s500mPWON\r" -i 1 -s "PWON\r"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Denon Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Denon Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1752
fi
