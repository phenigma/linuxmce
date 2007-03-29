#!/bin/bash

echo "Onkyo/Integra Detection script queue $2"

/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "!1PWRQSTN\r" -i 1 -s "PWR0"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Onkyo Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Onkyo Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1886
fi
