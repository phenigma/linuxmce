#!/bin/bash


echo Current display var: $DISPLAY
# DISPLAY_ID=`echo $DISPLAY | sed "s/^[^:]*://" | sed "s/\.[0-9]*$//"`

# NEXT_DISPLAY=`echo $DISPLAY_ID + 1 | bc`

NESTED_DISPLAY=$1
NESTED_DISPLAY_NAME=$2
NESTED_DISPLAY_GEOMETRY=$3

echo Using $NESTED_DISPLAY as the nested display name

COMMAND="`which Xnest` $NESTED_DISPLAY +kb -bw 0 -ac -name $NESTED_DISPLAY_NAME -geometry $NESTED_DISPLAY_GEOMETRY+0+0"


echo Running command: $COMMAND
$COMMAND &

echo Launching window manager on the nested display
DISPLAY=$NESTED_DISPLAY
COMMAND="`which ratpoison`"

echo Running command: $COMMAND
$COMMAND &
