#!/usr/bin/php5
<?
$incpath = "/var/www/recordings/includes";
include_once("$incpath/main.conf.php");
include_once("$incpath/crypt.php");

$crypt = new Crypt();
echo $crypt->encrypt($argv[1], $ARI_CRYPT_PASSWORD)."\n";
?>
