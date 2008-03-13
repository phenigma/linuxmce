#!/bin/bash
. /usr/pluto/bin/Utils.sh

AddBookmark "http://dcerouter/mythweb/" "MythWeb"

adduser mythtv public || :
mkdir -p /root/.mythtv && touch /root/.mythtv/ignoregroup || :
