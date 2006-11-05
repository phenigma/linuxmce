#!/bin/bash

# TV must be on, set to id 01, and 4800 or 9600.
# We have a couple of the same models here.  One defaulted to 4800.  The other to 9600.  And both returned different response strings, the only sequence in common was 00

echo "Runco Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 4800 -t "\0201GSO\03" -i 1 -s "00"
if [[ "$?" -ne 0 ]]; then
	echo "It's not a Runco TV at 4800 baud"
	/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "\0201GSO\03" -i 1 -s "00"
	if [[ "$?" -ne 0 ]]; then
		/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
	else
		echo "It is a Runco TV at 9600"
		/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 804 224 $2 52 78 5 9600
		/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1847
	fi
else
echo "It is a Runco TV at 4800"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 804 224 $2 52 78 5 4800
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1847
fi
