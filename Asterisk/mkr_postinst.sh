#!/bin/bash
echo 'CREATE DATABASE IF NOT EXISTS `asterisk`;' | mysql

# TODO: treat upgrades
mysql asterisk </usr/pluto/install/asterisk.sql
