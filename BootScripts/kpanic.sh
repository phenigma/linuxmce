#!/bin/bash
# setup reboot on Kernel Panic (3 seconds?)

echo "1" >/proc/sys/kernel/panic
