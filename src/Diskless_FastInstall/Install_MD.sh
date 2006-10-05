#!/bin/bash

mv /usr/sbin/invoke-rc.d{,.pluto-install}
echo -en '#!/bin/bash\necho "WARNING: fake invoke-rc.d called"\n' >/usr/sbin/invoke-rc.d
chmod +x /usr/sbin/invoke-rc.d

mount -t proc proc ./proc
mount 192.168.80.1:/usr/pluto/deb-cache/ /usr/pluto/deb-cache
echo 'exit 0' >/etc/default/hal
/usr/pluto/install/Initial_Config.sh
echo 'DAEMON_OPTS=' >/etc/default/hal
umount /usr/pluto/deb-cache
umount ./proc

mv /usr/sbin/invoke-rc.d{.pluto-install,}
