#!/bin/bash
# dump database to file

mysqldump --quote-names --allow-keywords --add-drop-table pluto_main >/var/log/complete_mysql_dump
