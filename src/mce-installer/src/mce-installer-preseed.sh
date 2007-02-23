#!/bin/bash

## Set deb-frontend to noninteractive
. /usr/share/debconf/confmodule
db_set "debconf/frontend" "Noninteractive"
db_fset "debconf/frontend" seen true
db_set "debconf/priority" "critical"
db_fset "debconf/priority" seen true
