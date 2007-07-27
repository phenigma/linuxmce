#!/bin/bash

ret=0

if [[ -x /bin/nc ]] ;then
	nc -z -w 1 www.google.com 80
	ret=$?
fi

exit $ret
