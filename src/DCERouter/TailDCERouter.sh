#!/bin/bash

cd /var/log/pluto
tail -f DCERouter.log.new | grep '^01\|^07\|^08'
