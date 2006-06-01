#!/bin/bash

echo "Gateway Detection script"

# as soon as the bug in serialport.read is fixed, mantis 2311, use the first one
#/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "\83\00\s1000m\83\01" -i 1 -s "\00\01"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "\83\00\s1000m\83\01" -i 1 -s "\01"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Gateway Plasma"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Gateway Plasma"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1750
fi
