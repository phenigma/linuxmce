#!/bin/bash

cd /var/log/pluto
tail -f DCERouter.newlog | grep '^01\|^07\|^08'

