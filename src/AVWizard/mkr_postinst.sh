#!/bin/bash

update-rc.d check_avwizard.sh start 98 S . >/dev/null
update-rc.d start_avwizard.sh start 99 S . >/dev/null

