#!/bin/bash
rm /tmp/MakeScript.log
bash -x /home/WorkNew/src/MakeRelease/MakeScript.sh "$@" 2>&1 | tee /tmp/MakeScript.log
