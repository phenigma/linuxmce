#!/bin/bash

nc -z -w 1 www.google.com 80
google_ok=$?


exit $google_ok
