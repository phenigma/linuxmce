<?php
 function adminHome($output) {
 		if ((!isset($_SESSION["adminIsLogged"]))||((int)$_SESSION["adminIsLogged"]==0)) {
			header("Location: index.php");
		}
		
 		$leftMenu = '	
			<table border=0>				
				<tr>
					<td class="text" valign="top" nowrap><a href="index.php?section=newsCategoriesManagement"><b>News Categs</b></a></td>
 				</tr>
 				<tr>
 					<td class="text" valign="top"><a href="index.php?section=newsManagement"><b>News</b></a></td>
 				</tr>
				<tr>
					<td class="text" valign="top" nowrap><a href="index.php?section=faqCategoriesManagement"><b>FAQ Categs</b></a></td>
 				</tr>
 				<tr>
 					<td class="text" valign="top"><a href="index.php?section=faqManagement"><b>FAQ</b></a></td>
 				</tr>
			</table>		
		';
		$out = '';
		
 		
 		$output->setNavigationMenu(array("Admin home"=>"index.php?section=adminHome"));
 		
		$output->setLeftMenu($leftMenu);
		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);
		$output->setTitle(APPLICATION_NAME."::Admin");			
  		$output->output(); 
 }
?>