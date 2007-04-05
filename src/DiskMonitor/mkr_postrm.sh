#!/bin/bash

## Remove the cron entry
#cat  /etc/crontab  | grep -v "DiskSpaceMonitor.sh" > /etc/crontab.$$
#mv /etc/crontab.$$ /etc/crontab
rm -f /etc/cron.d/DiskSpaceMonitor

