<?php
function clientLoggedOut($output) {
	
	$_SESSION['userIsLogged']='';
	session_unset();	
	session_destroy();	

	$out = '<div class="text" align="center" style="height:100px;vertical-align:middle;"><h3>Thank you for your visit</h3></div>';
			
	$output->setBody($out);
	$output->setNavigationMenu(array("User Area"=>'index.php?section=userLoginForm',"User Logged out"=>""));
	$output->setTitle(APPLICATION_NAME."::Thank you for your visit");
	$output->output();
}
?>