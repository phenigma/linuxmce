#!/bin/bash

["$!" == ""] && PORT=7890

SLIM_SERVER_HOME=`dirname $0`/../servers/SlimServer;
PARAMS="--audiodir /home/root -cliaddr 127.0.0.1 --cliport $PORT --d_protocol --d_cli --d_server";

exec `which perl` $SLIM_SERVER_HOME/slimserver.pl $PARAMS;

