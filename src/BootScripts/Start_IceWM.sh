#!/bin/bash

/usr/bin/icewm-session 2>&1 | tee-pluto -a /var/log/pluto/icewm.log
