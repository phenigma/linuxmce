<?php
function adminLoggedOut($output) {
	
	$out = '<span class="text"><h3>Thank you for your visit</h3></span>';
			
	$output->setBody($out);
	$output->setNavigationMenu(array("Admin Area"=>'index.php?section=adminLoginForm',"Admin Logged out"=>""));
	$output->setTitle(APPLICATION_NAME."::Thank you for your visit");
	$output->output();
}
?>