#!/bin/bash

echo "DSCPanel Power5020 Detection script"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "\30\30\30\39\30\0D\0A" -i 1 -s "\35\30\30\30\30\30\32\35\0D\0A"
if [[ "$?" -ne 0 ]]; then
echo "It's not a DSCPanel Power5020"
/usr/pluto/bin/MessageSend localhost -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a DSCPanel Power5020"
/usr/pluto/bin/MessageSend localhost -r 0 $1 1 806 224 $2 13 "$4" 44 1767
fi
