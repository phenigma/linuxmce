#!/bin/bash
# enable DMA on all disks

/sbin/hdparm -d1 -k1 /dev/hd[a-d] &>/dev/null

