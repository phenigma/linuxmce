<?php
function adminLoginForm($output) {
	
	$out = '
		
   
	<center><span class="tabletitle"><font color="black">Admin :: Log-in</font></span></center>
 

			<table cellSpacing="1" cellPadding="5" align="center" bgColor="#cccccc" border="0">
              		<tbody>
              			<tr>
                			<td width="100%" bgColor="#f5f5f5">
								<form action="index.php" method="post" name="loginForm">
								<input type="hidden" name="section" value="adminLoginLogout">
                  				<table height="80" cellSpacing="0" cellPadding="5" border="0">
                    			 <tbody>
                    				<tr>
                      					<td height="20">
											<span class="formtext">Username:&nbsp;</span></td>
                      					<td><input name="usernameA" class="long"></td>
									</tr>
                    				<tr>
                      					<td height="20"><span class="formtext">Password:&nbsp;</span></td>
                      					<td><input type="password" name="passwordA" class="long"></td>
									</tr>
                    				<tr>
                      					<td align="middle" colSpan="2"><input type="submit" value="Log-In" name="submitX" class="long"></td>
									</tr>
								</tbody>
							   </table>
							   </form>
							</td>
						</tr>
					</tbody>
				</table>	
	
	<center><span class="text">'.@$_GET['error'].'</span></center>
		
		';
			
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');
		$output->setScriptInBody(' onLoad="javascript:document.forms[\'loginForm\'].usernameA.focus();"');
		$output->setBody($out);
		$output->setTitle(APPLICATION_NAME);			
  		$output->output(); 
}
?>