#!/bin/bash

mysqldump --quote-names --allow-keywords --add-drop-table pluto_main > /var/log/pluto/pluto_main.dump.newlog
