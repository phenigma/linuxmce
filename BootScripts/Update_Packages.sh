#!/bin/bash

echo "Performing system update"
apt-get update
apt-get -y dist-upgrade
