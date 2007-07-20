<?
include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

print polywell_import($osDB,$conn,(int)@$_REQUEST['all']);

?>
