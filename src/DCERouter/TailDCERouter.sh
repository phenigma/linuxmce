#!/bin/bash

cd /var/log/pluto
tail -f DCERouter.log | grep '^01\|^07\|^08'
