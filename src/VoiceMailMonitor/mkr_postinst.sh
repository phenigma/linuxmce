#!/bin/bash

Content="#!/bin/bash

case \"\$1\" in
  start)
        screen -S VoiceMailMonitor -d -m /bin/bash /usr/pluto/bin/VoiceMailMonitor.sh
        ;;
  stop)
        ;;
  reload|force-reload)
        ;;
  restart)
        ;;
  *)
        echo \"Usage: /etc/init.d/voiceMailMonitor {start|stop|reload|force-reload|restart}\"
        exit 1
esac
"

echo "$Content" > /etc/init.d/voiceMailMonitor

chmod +x /etc/init.d/voiceMailMonitor || :

update-rc.d -f voiceMailMonitor start 99 2 3 4 5 . || /bin/true 
