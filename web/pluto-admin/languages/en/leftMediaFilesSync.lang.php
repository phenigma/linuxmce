<?
function leftMediaFilesSync($output,$mediadbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$startPath=(isset($_REQUEST['startPath']))?urldecode($_REQUEST['startPath']):'/home/public';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$out='';	
	
	if($action=='form'){
		$out='
		<script>
		function syncPath(path)
		{
			top.basefrm.location=\'index.php?section=mainMediaFilesSync&path=\'+escape(path);
			self.location=\'index.php?section=leftMediaFilesSync&startPath=\'+escape(path);
		}
		</script>
		<form action="index.php" method="POST" name="leftMediaFilesSync">
		<input type="hidden" name="section" value="leftMediaFilesSync">
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
		
		$linksTree='';
		$pathsArray=explode('/',$startPath);
		$pathTo='';
		$indent='';
		$childsArray=getDirectories ($startPath);
		foreach ($pathsArray AS $directory){
			if($directory!=''){
				$pathTo.='/'.$directory;
				$linksTree.=$indent.(($pathTo==$startPath)?'<B>[ '.stripslashes($directory).' ]</B><br>':'<a href="javascript:syncPath(\''.urlencode($pathTo).'\')"><B>&gt; '.stripslashes($directory).'</B></a><br>');
				$indent.='&nbsp;&nbsp;&nbsp;&nbsp;';
			}
		}
		
		foreach ($childsArray AS $childPath=>$childName){
			$linksTree.=$indent.'<a href="javascript:syncPath(\''.urlencode(addslashes($childPath)).'\')">&gt; '.stripslashes($childName).'</a><br>';
		}

		
		$out.='
			<tr>
				<td colspan="2">'.$linksTree.'</td>
			</tr>
			</table>
		</form>';
	}else{
		// processing area
	}	

	
	$output->setScriptInBody('bgColor="#F0F3F8"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}

function getDirectories ($path) {
	$dirs=array();
	if (($d = @opendir ($path)) === false) 
		return $dirs;
	else {
		while ($f = readdir ($d)) {
			if ($f != "." && $f != "..") {
				if (@is_dir ($path .'/'.$f)) {
					$dirs[$path.'/'.$f]=$f;
				} 
			}
		}
		closedir ($d);
	}
	return $dirs;
}
?>