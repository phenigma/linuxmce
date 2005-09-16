#!/bin/bash

sh -x /var/www/RemoveLinks.sh
ln -sf /var/www/pluto-support /var/www/plutohome-com/support
ln -sf /var/www/download /var/www/plutohome-com/download
ln -sf /var/www/trunk_doc /var/www/pluto-support/doxygen
ln -sf /var/www/pluto-support-modified/mantis /var/www/pluto-support/mantis
ln -sf /var/www/pluto-support-modified/phpbb2 /var/www/pluto-support/phpbb2
ln -sf /var/www/pluto-support/pluto_header.php /var/www/trunk_doc/pluto_header.php
ln -sf /var/www/pluto-admin /var/www/plutohome-com/support/plutoadmin
ln -sf /var/www/pluto-admin /var/www/plutohome-com
ln -sf /var/www/SugarSuite /var/www/plutohome-com
#ln -sf /var/www/debian /var/www/plutohome-come/debian
