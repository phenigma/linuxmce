#!/bin/bash

echo "Runco VX2c queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 19200 -t "\rPOWER?\r" -i 1 -s "ON" -s "OFF" -s "on" -s "off"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Runco VX2c"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a RuncoVX2c"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1865
fi
