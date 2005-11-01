#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

Q="SELECT Icon FROM QuickStartTemplate"
R=$(RunSQL "$Q")

for Icon in $R; do
	#wget "$Icon" # where to put this?
done
