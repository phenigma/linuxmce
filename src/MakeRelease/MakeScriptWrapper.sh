#!/bin/bash
rm /tmp/MakeScript.log
sh -x /home/WorkNew/src/MakeRelease/MakeScript.sh $1 2>&1 | tee /tmp/MakeScript.log
