#!/bin/bash

nice -n 19 mysqldump --quote-names --allow-keywords --add-drop-table pluto_main > /var/log/pluto/pluto_main.dump.newlog
