#!/bin/bash

echo -e "\n\n" | `which mythtv-setup` &

sleep 1;
./ratpoison -c ":select mythtv-setup";
