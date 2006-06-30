#!/bin/bash

echo "Denon Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "PWSTANDBY\r\s500mPWON\r" -i 1 -s "PWON\r"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Denon Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Denon Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1752
fi
