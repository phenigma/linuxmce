#!/bin/bash

. /home/WorkNew/src/BootScripts/SQL_Ops.sh
UseDB pluto_builder

Q="
	SELECT Filename
	FROM File
"
while read row; do
	filename=$(Field 1 "$row")
	filemd5=$(md5sum "$filename" | awk '{print $1}')
	echo "MD5: $filemd5; File: $filename"
	Q="
		UPDATE File
		SET md5='$filemd5'
		WHERE Filename='$filename'
	"
	RunSQL "$Q"
done < <(RunSQL "$Q" | tr ' ' '\n')
