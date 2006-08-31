#!/bin/bash

/usr/bin/icewm --debug --debug-z 2>&1 | tee -a /var/log/pluto/icewm.log
