#!/bin/bash

MAC="$1"
[ -z "$MAC" ] && exit 1
[ "${#MAC}" -ne 17 ] && exit 1


ValidDigits="0123456789ABCDEFabcdef:"

MAC="${MAC//-/:}"
for ((i = 0; i < ${#MAC}; i++)); do
	Ch="${MAC:$i:1}"
	[ "$((i % 3 == 2))" -eq 1 -a "$Ch" != ":" ] && exit 1
	[ "${ValidDigits/$Ch}" == "$ValidDigits" ] && exit 1
done

exit 0
