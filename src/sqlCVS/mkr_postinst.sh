#!/bin/bash

PrevVer="$2"

if [[ -n "$PrevVer" ]]; then
	/usr/bin/php /usr/pluto/bin/sqlcvs_db_fixer.php
fi
