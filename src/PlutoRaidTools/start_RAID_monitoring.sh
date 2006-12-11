#!/bin/bash

echo "PROGRAM /usr/pluto/bin/monitoring_RAID.sh" >> /etc/mdadm/mdadm.conf

mdadm --monitor --scan --daemonise


