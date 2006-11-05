#!/bin/bash

echo "Wavetrend RFID queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 115200 -t "\aa\00\00\00\01\03\02" -i 1 -s "\aa\00"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Wavetrend RFID"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Wavetrend RFID"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1635
fi
