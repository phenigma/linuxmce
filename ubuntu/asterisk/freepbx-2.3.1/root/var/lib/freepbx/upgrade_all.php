#!/usr/bin/php -q
<?php
define("BASEDIR", "/var/lib/freepbx");
require_once (BASEDIR . "/libfreepbx.install.php");

define("UPGRADE_DIR", BASEDIR . "/upgrades");
define("MODULE_DIR", "/var/www/admin/modules");

function out($text) {
	echo $text."\n";
}

function outn($text) {
	echo $text;
}

function error($text) {
	echo "[ERROR] ".$text."\n";
}

function fatal($text) {
	echo "[FATAL] ".$text."\n";
	exit(1);
}

function debug($text) {
	global $debug;
	
	if ($debug) echo "[DEBUG-preDB] ".$text."\n";
}

function version_compare_freepbx($version1, $version2) {
	$version1 = str_replace("rc","RC", strtolower($version1));
	$version2 = str_replace("rc","RC", strtolower($version2));
	return version_compare($version1, $version2);
}

/** Set base of packaged modules to the versions packaged in the tarball since they are
 *  getting overwritten with the tarball from anything that may have been updated online.
 *
 */
function set_base_version() {
	global $dryrun;

	// read modules list from MODULE_DIR
	//
	$included_modules = array();
	$dir = opendir(MODULE_DIR);
	while ($file = readdir($dir)) {
		if ($file[0] != "." && $file[0] != "_" && is_dir(MODULE_DIR."/".$file)) {
			$included_modules[] = $file;
		}
	}
	closedir($dir);

	foreach ($included_modules as $up_module) {
		outn("Checking $up_module.. ");
		if (!$dryrun) {
			out(set_module_version($up_module));
		} else {
			out("Dry Run Not Updated");
		}
	}
}

function set_module_version($module) {
	global $db;

	$module_dir = MODULE_DIR;
	$file_path = $module_dir.$module."/module.xml";
	if (file_exists($file_path)) {
		// TODO: this is bad, there are other version tags (depends on) but this
		//       is equivalnet to what publish.pl does, so it expects this to be
		//       at the top.
		//
		$module_xml = file_get_contents($file_path);
		if (preg_match('/<version>(.+)<\/version>/', $module_xml, $matches)) {
			$version = $matches[1];
		} else {
			fatal("ERROR: $file_path found but no verison information");
		}
	} else {
		return  "not packaged, no updating needed";
	}

	// If we didn't return above, then we found the package as part of the install
	// tarball and want to update the version info since this might be overwriting
	// and existing install that has a newever version.
	//
	$sql = "SELECT version FROM modules WHERE modulename = '$module'";
	$result = $db->getCol($sql); 
	if(DB::IsError($result)) {     
		fatal("error accessing version table: ".$result->getMessage()); 
	}
	$sql = "";
	if (count($result) == 0) {
		$sql = "INSERT INTO modules (modulename, version, enabled) VALUES ('$module', '$version', 1)";
	} else if ($result[0] != $version) {
		$sql = "UPDATE modules SET version = '$version', enabled = 1 WHERE modulename = '$module'";
	}
	if ($sql) {
		debug($sql);
		$result = $db->query($sql); 
		if(DB::IsError($result)) {     
			fatal("error writing to version table: ".$result->getMessage()); 
		}
		return "updated to $version";
	} else {
		return "already at $version";
	}
}

function generate_configs() {
	global $dryrun;
	global $debug;
	
	out("Generating Configurations.conf, (if Asterisk is not running, you will get an error");
	out("In case of error, start Asterisk and hit the red bar in the GUI to generate the Configuraions.conf files");
	if (!$dryrun)
		// added --run-install to make it work like it has been working since retrieve_conf changed to not run module install scripts by default

		//
		// TODO: Should check if Asterisk is running and/or try to start it.
		//
		passthru("su - asterisk -c \"/var/lib/asterisk/bin/retrieve_conf --run-install ".($debug ? ' --debug' : '').'"');
}

$db = DB::connect("mysql://asteriskuser:amp109@localhost/asterisk");
if (DB::isError($db))
	fatal("Failed to connect to database");

upgrade_all($argv[1]);
?>
