#!/bin/bash

## Remove Update_Packages.sh from crontab
sed -i '\,/usr/pluto/bin/Update_Packages\.sh, d' /etc/crontab
