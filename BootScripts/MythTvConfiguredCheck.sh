#!/bin/bash

QueryIsConfigured="SELECT count(data) as a FROM settings WHERE value like '%MasterServer%'";
#MysqlCommand="mysql -D mythconverg -u mythtv -pArkhNKpr";
MysqlCommand="mysql -D mythconverg";

QueryResult=`echo $QueryIsConfigured | $MysqlCommand | tail -n 1`;

if [ $QueryResult ]; then
        echo "Need to launch the setup";
        echo -e "\n\n" | mythtv-setup;

        chvt 1;

        echo "Please wait until MythTv backend server completes updating the channel database";
        /etc/cron.daily/mythtv-backend;
        echo "Completed";
fi;

/etc/init.d/mythtv-backend force-reload;


# looking for XServer pid

# XServerPid=`lsof | grep /tmp/.X11-unix/X0 | head -n 1 | sed "s/^[^ ]* *//" | cut -f 1 -d ' '`;
# echo "XServer pid: $XServerPid";

chvt 7;


