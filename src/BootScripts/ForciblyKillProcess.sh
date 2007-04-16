#!/bin/bash

# Takes as a parameter the name of a program, and it will forcibly kill it, first by sending
# the processes a kill, and then a kill -9 if they don't die

killall -9 "$1"
