#!/bin/bash
echo 'CREATE DATABASE IF NOT EXISTS `eib`;' | mysql -h dcerouter
echo 'CREATE TABLE IF NOT EXISTS `groupaddresses` (
`name` varchar(100) default NULL,
`address` varchar(100) NOT NULL,
PRIMARY KEY  (`address`)
);' | mysql eib -h dcerouter

# add eib user
adduser --system --home /var/run/eib eib || true
