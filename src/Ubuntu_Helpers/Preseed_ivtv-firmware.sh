#!/bin/bash

. /usr/share/debconf/confmodule

db_fset ivtv/present-hauppauge-eu-v1 seen true

db_set ivtv/accepted-hauppauge-eu-v1 'true'
db_fset ivtv/accepted-hauppauge-eu-v1 seen true
