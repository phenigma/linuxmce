<?
function leftMediaBrowser($output,$dbADO) {
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$out='';	
	
	if($action=='form'){
		$out='
		<form action="index.php" method="POST" name="leftMediaBrowser">
		<input type="hidden" name="section" value="leftMediaBrowser">
		<input type="hidden" name="action" value="search">
		
		<table border=0 align="left">				
				<tr>
					<td colspan="2">
						<img src="include/images/logo_pluto.jpg">
					</td>
				</tr>';
		
		if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {
			$out.='
					<tr>
						<td valign="top"  colspan="2"><a href="index.php?section=login&action=logout" target="basefrm" >Logout</a></td>					
					</tr>
			';
		} else {
			$out.='
					<tr>
						<td valign="top"  colspan="2"><a href="index.php?section=login"  target="basefrm">Login</a></td>
					</tr>
			';
		}
		$out.='
		<tr>
			<td colspan="2"><B>Search</B></td>
		</tr>
		<tr>
			<td><input type="text" name="searchString" value="'.@$_SESSION['mediaSearchString'].'"></td>
			<td><input type="submit" class="button" name="go" value="Go"></td>
		</tr>
		<tr>
			<td colspan="2"><hr><B>Jump</B></td>
		</tr>
		<tr>
			<td colspan="2" align="center"><a href="#" onClick="javascript:setSearch(\'none\');">#1-9</a></td>
		</tr>
		';
		for($i=65;$i<91;$i++){
			$out.='
			<tr>
				<td colspan="2" align="center"><a href="#" onClick="javascript:setSearch(\''.chr($i).'\');">'.chr($i).'</a></td>
			</tr>
			';	
		}
		$out.='</table>
		</form>';
	}else{
		if(isset($_POST['go'])){
			$_SESSION['mediaSearchString']=$_POST['searchString'];
			$out.='
			<script>
				setSearch(\'\');
			</script>';
		}
		$out.='
			<script>
				self.location=\'index.php?section=leftMediaBrowser\';
			</script>';
	}	

	$scriptInHead=
		'<script>
		function setSearch(value)
		{
			if(!top.basefrm.document.mainMediaBrowser)
				eval(\'top.basefrm.location="index.php?section=mainMediaBrowser&searchType="+value\');
			else{
				top.basefrm.document.mainMediaBrowser.searchType.value=value;
				top.basefrm.document.mainMediaBrowser.submit();
			}
		}
		</script>';
	
	$output->setScriptInHead($scriptInHead);	
	$output->setScriptInBody('bgColor="#F0F3F8"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}


?>
