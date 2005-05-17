#!/bin/bash

TimeZone="$1"

File="/usr/share/zoneinfo/$TimeZone"
[[ -z "$TimeZone" || ! -f "$File" ]] && exit 1

rm -f /etc/localtime
ln -s "$File" /etc/localtime
echo "$TimeZone" >/etc/timezone
