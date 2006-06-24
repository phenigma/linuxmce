#!/bin/bash

echo "Mitsubishi Plasma Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P O81 -b 9600 -t "\9f\80\60\4e\00\cd" -i 3 -s "\80\4e\00"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Mitsubishi Plasma"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Mitsubishi Plasma"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1774
fi
