#!/bin/bash

while ./bin/ping dcerouter &>/dev/null; do
	./bin/true
done
./bin/chvt 11
