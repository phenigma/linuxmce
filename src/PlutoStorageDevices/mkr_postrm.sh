#!/bin/bash

## Remove the cron entry
cat  /etc/crontab  | grep -v "StorageDevices_Radar.sh" > /etc/crontab.$$
mv /etc/crontab.$$ /etc/crontab

