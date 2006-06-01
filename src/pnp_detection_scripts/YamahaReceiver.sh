#!/bin/bash

echo "Yamaha Detection script"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "\s1000m\11000\03\s1000m\11000\03" -i 1 -s "AF"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Yamaha Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Yamaha Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1752
fi
