#!/bin/bash

cd /var/log/pluto
grep '^01\|^07\|^08' DCERouter.log | less -R
