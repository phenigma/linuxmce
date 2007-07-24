#!/bin/bash

if [ -e /tmp/PlayedMedia.tmp ] ;then
        rm -f /tmp/PlayedMedia.tmp
fi

echo >> /PlayedMedia.log
echo "*PLAYED MEDIA*" >> /PlayedMedia.log
echo "==============" >> /PlayedMedia.log
echo "PlayedMedia.sh" >> /tmp/status.log

echo >> /PlayedMedia.log

nr=$(cat /var/log/pluto/*_Xine_Player.log | grep "Xine_Player::CMD_Play_Media() called for" | wc -l)
echo "---Xine_Player::CMD_Play_Media() [$nr]" >> /PlayedMedia.log

echo >> /PlayedMedia.log
