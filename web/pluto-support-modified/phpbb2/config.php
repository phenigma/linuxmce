<?php

/* DO NOT CHANGE THIS FILE

This file is part of the Debian package for phpbb. To configure phpbb2,
edit /etc/phpbb2/config.php.

For using multiple phpbb boards on this computer with different databases,
include 'php_value auto_prepend_file /etc/phpbb2/alternative_config.php'
in the corresponding <Location> or <VirtualHost> section of your apache's config.

That file will then override the /etc/phpbb2/config.php file

*/

if (!defined('PHPBB_INSTALLED')) {
	if (!file_exists('/etc/phpbb2/config.php')) {
		die("You need to install a configuration file in
		<tt>/etc/phpbb2/config.php</tt> to tell phpbb where its database can
		be found. Install phpbb2-conf-mysql to have a database setup
		automatically, or see <tt>/usr/share/doc/phpbb2/README.Debian</tt> for
		information on how to make one yourself.");
	}
	require '/etc/phpbb2/config.php';
	if (!defined('PHPBB_INSTALLED')) {
		die("Config file incorrect - see 
			<tt>/usr/share/doc/phpbb2/README.Debian.gz</tt>");
	}
}

?>
