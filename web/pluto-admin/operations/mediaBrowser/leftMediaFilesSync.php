<?
function leftMediaFilesSync($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$startPath=(isset($_REQUEST['startPath']))?urldecode($_REQUEST['startPath']):'/home/public/data';
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
		<input type="hidden" name="startPath" value="'.urlencode($startPath).'">
		
		<table border=0 align="left">				
				<tr>
					<td colspan="2">
						<a href="index.php" target="_top"><img src="include/images/logo_pluto.jpg?rand='.rand(1000,9999).'" border="0"><a>
					</td>
				</tr>';
		
		if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {
			$out.='
					<tr>
						<td valign="top"  colspan="2"><a href="index.php?section=login&action=logout" target="basefrm" >'.$TEXT_LOGOUT_CONST.'</a></td>					
					</tr>
			';
		} else {
			$out.='
					<tr>
						<td valign="top"  colspan="2"><a href="index.php?section=login"  target="basefrm">'.$TEXT_LOGIN_CONST.'</a></td>
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
				$linksTree.=$indent.(($pathTo==$startPath)?'<B>[ <a href="javascript:syncPath(\''.urlencode($pathTo).'\')"><B> '.stripslashes($directory).'</B></a> ]</B><br>':'<a href="javascript:syncPath(\''.urlencode($pathTo).'\')"><B>&gt; '.stripslashes($directory).'</B></a><br>');
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
			<tr>
				<td colspan="2"><input type="checkbox" name="UpdateMediaDaemon" value="1" '.((get_update_media_daemon_status($dbADO)==1)?'checked':'').' onClick="document.leftMediaFilesSync.submit();"> '.$TEXT_UPDATE_MEDIA_DAEMON_CONST.'</td>
			</tr>		
			</table>
		</form>';
	}else{
		// processing area
		set_update_media_daemon_status((int)@$_REQUEST['UpdateMediaDaemon'],$dbADO);

		header('Location: index.php?section=leftMediaFilesSync&startPath='.urlencode($_REQUEST['startPath']));
		exit();
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
	asort($dirs,SORT_STRING);
	reset($dirs);
	
	return $dirs;
}

function get_update_media_daemon_status($dbADO){
	$coreDevices=array_keys(getDevicesArrayFromCategory($GLOBALS['CategoryCore'],$dbADO));
	$scriptInfo=getAssocArray('Device_StartupScript','FK_StartupScript','Enabled',$dbADO,'WHERE FK_Device='.$coreDevices[0].' AND FK_StartupScript='.$GLOBALS['UpdateMediaDaemon']);
	
	return @$scriptInfo[$GLOBALS['UpdateMediaDaemon']];
}

function set_update_media_daemon_status($value,$dbADO){
	$coreDevices=array_keys(getDevicesArrayFromCategory($GLOBALS['CategoryCore'],$dbADO));
	$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=? WHERE FK_Device=? AND FK_StartupScript=?',array($value,$coreDevices[0],$GLOBALS['UpdateMediaDaemon']));
	
	$cmd='sudo -u root /usr/pluto/bin/generateRcScripts.sh';
	exec($cmd);
}
?>