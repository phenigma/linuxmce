<?
require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/weborbiter.inc.php');

// retrieve variables
$userID=(int)@$_REQUEST['userID'];
$passMD5=@$_REQUEST['pass'];
$installationID=(int)@$_REQUEST['installationID'];
$deviceID=(int)@$_REQUEST['deviceID'];



// check if user is logged
if(is_valid_user($userID,$passMD5,$dbADO)){
	$out=get_web_orbiter($installationID,$deviceID,$userID,$passMD5,$dbADO);
}else{
	$out=login_form($dbADO);
}

print $out;
?>