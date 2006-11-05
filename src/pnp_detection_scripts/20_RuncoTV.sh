#!/bin/bash

# TV must be on, set to id 01, and 4800

echo "Runco Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 4800 -t "\0201GSO\03" -i 1 -s "PLASMA"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Runco TV"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Runco TV"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1847
fi
