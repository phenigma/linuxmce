#!/bin/bash

update-rc.d check_avwizard start 98 S . >/dev/null
update-rc.d start_avwizard start 99 S . >/dev/null

