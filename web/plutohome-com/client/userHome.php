<?
function userHome($output) {
 		if ($_SESSION['userIsLogged']!="yes") {
			header("Location: index.php");
		}
      $out = '';
		
 		
 		$output->setNavigationMenu(array("Client home"=>"index.php?section=userHome"));
 		
		$output->setLeftMenu($leftMenu);
		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);
		$output->setTitle(APPLICATION_NAME."::Client");			
  		$output->output(); 
}
?>