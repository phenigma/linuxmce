#!/bin/bash
trap './bin/chvt 11' EXIT

while ./bin/ping -c 1 dcerouter &>/dev/null; do
	./bin/sleep 5
done
