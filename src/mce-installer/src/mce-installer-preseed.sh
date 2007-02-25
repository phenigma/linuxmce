#!/bin/bash

## Set deb-frontend to noninteractive
. /usr/share/debconf/confmodule
db_set "debconf/frontend" "Noninteractive"
db_fset "debconf/frontend" seen true
db_set "debconf/priority" "critical"
db_fset "debconf/priority" seen true

## Set the ivtv-firmware license as 'accepted'
db_fset ivtv/present-hauppauge-eu-v1 seen true
db_set ivtv/accepted-hauppauge-eu-v1 'true'
db_fset ivtv/accepted-hauppauge-eu-v1 seen true
