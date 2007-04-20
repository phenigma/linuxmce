#!/bin/bash

counter=30
while ((counter > 0)); do
	nice -n 20 /usr/pluto/bin/UpdateMedia -h localhost -d /home/public/data -B -t | tee-pluto -a /var/log/pluto/UpdateMedia.log
	((counter--))
done
