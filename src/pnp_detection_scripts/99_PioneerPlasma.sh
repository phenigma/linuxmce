#!/bin/bash

echo "device $1"
echo "queue $2"
echo "path $3"
echo "name $4"

#failure
#/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 18 "test error"

#success
#/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1752

#no match
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
