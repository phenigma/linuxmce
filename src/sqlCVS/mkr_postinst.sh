#!/bin/bash

PrevVer="$2"

if [[ -n "$PrevVer" ]]; then
	/usr/bin/php -c /etc/php4/apache2/php.ini /usr/pluto/bin/sqlcvs_db_fixer.php
fi
