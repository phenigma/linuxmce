#!/bin/bash

valgrind --leak-check=full --log-file=test_vg ./mysql_wrapper -h localhost -u root -D pluto_test -s /tmp/db_wrapper_socket

