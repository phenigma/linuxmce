<?php
 function samplePage($output) {
		$out = '';
		
$out.=<<<TEXT

		put any text in here
		
TEXT;
		
		$output->setImgName("img1.jpg"); //img that needs to be changed
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Sample page");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>