#!/bin/bash
echo "Starting X server"

screen -d -m -S XWindowSystem X -ac
sleep 5

