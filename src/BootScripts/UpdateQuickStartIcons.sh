#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

Q="SELECT Icon FROM QuickStartTemplate"
R=$(RunSQL "$Q")

for Icon in $R; do
	#TODO: when inmplementing the wget, don't forget the OfflineMode flag
	#wget "$Icon" # where to put this?
done
