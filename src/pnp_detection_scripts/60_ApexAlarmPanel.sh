#!/bin/bash

echo "Apex Detection script  queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 1200 -t "\b\s100m08as0064\r\n\s100m08as0064\r\n\s100m08as0064\r\n" -i 1 -s "10AS"
if [[ "$?" -ne 0 ]]; then
echo "It's not a apex"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a apex"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1781
fi
