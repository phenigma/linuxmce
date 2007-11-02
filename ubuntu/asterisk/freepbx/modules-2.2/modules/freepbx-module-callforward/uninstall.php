<?php

require_once('common/php-asmanager.php');

// remove all Call Forward options in effect on extensions
$astman = new AGI_AsteriskManager();
if ($res = $astman->connect("127.0.0.1", $amp_conf["AMPMGRUSER"] , $amp_conf["AMPMGRPASS"])) {

	$astman->database_deltree('CF');
	$astman->database_deltree('CFB');
	$astman->database_deltree('CFU');

} else {

	fatal("Cannot connect to Asterisk Manager with ".$amp_conf["AMPMGRUSER"]."/".$amp_conf["AMPMGRPASS"]);

}
$astman->disconnect();

?>
