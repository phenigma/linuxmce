#!/bin/bash
while : ;do
	echo "$(date -R) Started" >> /var/log/loop-build.log
	/usr/local/lmce-build/build.sh
	echo "$(date -R) Done" >> /var/log/loop-build.log
	sleep 60
done
