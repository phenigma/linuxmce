#!/bin/bash

echo "Enabling packet forwarding"
echo 1 >/proc/sys/net/ipv4/ip_forward
