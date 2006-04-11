<?
function mainMediaFilesSync($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mainMediaFilesSync.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$path = (isset($_REQUEST['path']) && $_REQUEST['path']!='')?cleanString($_REQUEST['path']):'';

	$typeArray=array();
	$resType=$dbADO->Execute('SELECT * FROM MediaType ORDER BY Description ASC');
	while($rowType=$resType->FetchRow()){
		$typeArray[$rowType['PK_MediaType']]=$rowType['Description'];
	}
	$notInDBArray=array();	
	$oldDir=substr($path,strrpos($path,'/')+1);
	
	$_SESSION['missing']=isset($_SESSION['missing'])?$_SESSION['missing']:0;
	$_SESSION['missing']=isset($_REQUEST['missing'])?$_REQUEST['missing']:$_SESSION['missing'];

	if($action=='form'){
		if($path!=''){
			$physicalFiles=grabFiles($path,'');

			$out.='
			<script>
			function windowOpen(locationA,attributes) 
			{
				window.open(locationA,\'\',attributes);
			}
			
			function requestName(oldDir)
			{
				var newDir=prompt("'.$TEXT_REQUEST_DIRECTORY_NAME_CONST.' ",oldDir);
				if(newDir!=oldDir && newDir!=""){
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($path).'&action=renDir&newDir="+newDir;
				}
			}
			
			function confirmDel()
			{
				if(confirm("'.$TEXT_CONFIRM_DELETE_DIRECTORY_CONST.'")){
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($path).'&action=delDir";
				}
			}
			
			function createSubdir()
			{
				var subDir=prompt("'.$TEXT_REQUEST_SUBDIRECTORY_NAME_CONST.' ","");
				if(subDir!=""){
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($path).'&action=newDir&subDir="+subDir;
				}
			}

			function syncPath(path)
			{
				top.treeframe.location=\'index.php?section=leftMediaFilesSync&startPath=\'+escape(path);
				self.location=\'index.php?section=mainMediaFilesSync&path=\'+escape(path);
			}
			</script>
			
			<a href="javascript:syncPath(\''.substr($path,0,strrpos($path,'/')).'\')">Up one level</a>
			<table cellpadding="3" cellspacing="0">
				<tr bgcolor="#F0F3F8">
					<td><B>'.$TEXT_DIRECTORY_CONST.': '.$path.'</B></td>
					<td><input type="button" class="button" name="resyncDir" value="'.$TEXT_RESYNCHRONIZE_CONST.'" onClick="windowOpen(\'operations/logs/executeLog.php?script=3&path='.urlencode($path).'\',\'width=1024,height=768,toolbars=true,scrollbars=1,resizable=1\');self.location=\'index.php?section=mainMediaFilesSync&action=resync&path='.$path.'\';"></td>
					<td><input type="button" class="button" name="renameDir" value="'.$TEXT_RENAME_CONST.'" onClick="requestName(\''.addslashes($oldDir).'\')"></td>
					<td><input type="button" class="button" name="subDir" value="'.$TEXT_CREATE_SUBDIRECTORY_CONST.'" onClick="createSubdir()"></td>
					<td><input type="button" class="button" name="delDir" value="'.$TEXT_DELETE_CONST.'" onClick="confirmDel();"></td>
				</tr>
			</table>
			
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div><br>
			<div align="center" class="err"><B>'.@$_REQUEST['error'].'</B></div><br>
			<form action="index.php" method="POST" name="mainMediaFilesSync">
			<input type="hidden" name="section" value="mainMediaFilesSync">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="path" value="'.$path.'">
			
			';
			if(isset($_REQUEST['filename']) && $_REQUEST['filename']!='')
				$out.='<a href="index.php?section=leftMediaFilesSync" target="treeframe"><img src="scripts/treeview/diffDoc.gif" border="0" align="middle">'.$TEXT_SHOW_DIRECTORY_STRUCTURE_CONST.'</a>';
			$out.='
				<table cellpading="0" cellspacing="0">
					<tr>
						<td colspan="2"><B>'.$TEXT_FILES_ON_DISK_CONST.'</B></td>
					</tr>';
			$queryDBFiles='SELECT * FROM File WHERE Path=?';
			$rs=$mediadbADO->Execute($queryDBFiles,$path);
			$dbFiles=array();
			$dbPKFiles=array();
			while($row=$rs->FetchRow()){
				$dbFiles[]=$row['Filename'];
				$dbPKFiles[]=$row['PK_File'];
			}
			if(count($physicalFiles)==0)
				$out.='	<tr>
							<td colspan="2">'.$TEXT_NO_FILES_IN_PHYSICAL_DIRECTORY_CONST.'</td>
						</tr>';
			else{
				foreach($physicalFiles as $physicalkey => $filename){
					if((in_array($filename,$dbFiles))){
						$key=array_search($filename,$dbFiles);
						$inDB=1;
					}else{
						$notInDBArray[]=$physicalkey; 
						$inDB=0;
						if(!is_dir($path.'/'.$filename)){
							$addToDB='<td align="right"> '.$TEXT_TYPE_CONST.': <select name="type_'.$physicalkey.'">
								<option value="">- '.$TEXT_PLEASE_SELECT_CONST.' -</option>';
							foreach ($typeArray AS $typeID=>$typeDescription){
								$addToDB.='<option value="'.$typeID.'">'.str_replace('np_','',$typeDescription).'</option>';
							}
							$addToDB.='</select> <input type="submit" class="button" name="add" value="'.$TEXT_ADD_FILE_TO_DATABASE_CONST.'">
							<input type="hidden" name="filename_'.$physicalkey.'" value="'.$filename.'">
							</td>';
						}else{
							$addToDB='<td>&nbsp;</td>';
						}
					}
					$out.='	<tr style="background-color:'.(($physicalkey%2==0)?'#EEEEEE':'#EBEFF9').';">
								<td '.(((@$inDB==1)?'colspan="2"':'')).'>'.((@$inDB==1)?'<img src=include/images/sync.gif align=middle border=0>':'<img src=include/images/disk.gif align=middle border=0>').' '.((@$inDB==1)?'<a href="index.php?section=editMediaFile&fileID='.$dbPKFiles[$key].'"><B>'.$filename.'</B></a>':'<B>'.$filename.'</B> '.$addToDB).'</td>
							</tr>';
						if(@$inDB==1){	
							$queryAttributes='
								SELECT PK_Attribute, AttributeType.Description AS AttributeName,Name
								FROM File_Attribute
									INNER JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute
									INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
								WHERE FK_File=? ORDER BY AttributeType.Description ASC
							';
							$resAttributes=$mediadbADO->Execute($queryAttributes,$dbPKFiles[$key]);
							$attributes='';
							while($rowAttributes=$resAttributes->FetchRow()){
								$attributes.='<b>'.$rowAttributes['AttributeName'].'</b>: <a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties" target="_self">'.stripslashes($rowAttributes['Name']).'</a> ';
							}
							$out.='
							<tr style="background-color:'.(($physicalkey%2==0)?'#EEEEEE':'#EBEFF9').';">
								<td colspan="2">'.@$attributes.'</td>
							</tr>';
							
						}
				}
			}
			if($_SESSION['missing']==1){
				$out.='
					<tr>
						<td colspan="2"><B>'.$TEXT_FILES_IN_DATABASE_ONLY_CONST.'</B></td>
					</tr>';
				$dbonlyFiles=array_diff($dbFiles,$physicalFiles);
				if(count($dbonlyFiles)==0)
					$out.='	<tr>
								<td colspan="2">'.$TEXT_NO_OTHER_FILES_IN_DATABASE_CONST.'</td>
							</tr>';
				foreach($dbonlyFiles as $dbkey => $filename){
					$queryAttributes='
						SELECT PK_Attribute, AttributeType.Description AS AttributeName,Name
						FROM File_Attribute
							INNER JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute
							INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
						WHERE FK_File=? ORDER BY AttributeType.Description ASC
						';
					$resAttributes=$mediadbADO->Execute($queryAttributes,$dbPKFiles[$dbkey]);
					$attributes='';
					while($rowAttributes=$resAttributes->FetchRow()){
						$attributes.='<b>'.$rowAttributes['AttributeName'].'</b>: <a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties" target="_self">'.stripslashes($rowAttributes['Name']).'</a> ';
					}
		
					$out.='	<tr style="background-color:'.(($dbkey%2==0)?'#EEEEEE':'#EBEFF9').';">
								<td><img src=include/images/db.gif align=middle border=0> <a href="index.php?section=editMediaFile&fileID='.@$dbPKFiles[$dbkey].'"><B>'.$filename.'</B></a></td>
								<td align="right"><input type="button" class="button" name="del" value="'.$TEXT_DELETE_FILE_FROM_DATABASE_CONST.'" onClick="if(confirm(\''.$TEXT_CONFIRM_DELETE_FILE_FROM_DATABASE_CONST.'\'))self.location=\'index.php?section=mainMediaFilesSync&action=delfile&dfile='.@$dbPKFiles[$dbkey].'&path='.$path.'\';"></td>
							</tr>
							<tr style="background-color:'.(($dbkey%2==0)?'#EEEEEE':'#EBEFF9').';">
								<td colspan="2">'.@$attributes.'</td>
							</tr>';
				}
			}
			$out.='
				<tr>
					<td colspan="2">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="2"><B>'.strtoupper($TEXT_LEGEND_FOR_FILES_CONST).'</B><br>
					<img src=include/images/disk.gif align=middle border=0>	'.$TEXT_EXIST_ONLY_ON_DISK_CONST.'<br>
					'.(($_SESSION['missing']==1)?'<img src=include/images/db.gif align=middle border=0> '.$TEXT_EXIST_ONLY_IN_DATABASE_CONST.'<br>':'').'
					<img src=include/images/sync.gif align=middle border=0>	'.$TEXT_EXIST_BOTH_ON_DISK_AND_IN_DATABASE_CONST.'<br>
					<input type="checkbox" name="show_missing" value="1" onclick="self.location=\'index.php?section=mainMediaFilesSync&path='.$path.'&missing='.((@$_SESSION['missing']==1)?0:1).'\'" '.(($_SESSION['missing']==1)?'checked':'').'> '.$TEXT_SHOW_FILES_WHO_ARE_MISSING_FROM_DISK_CONST.'
				</tr>
			</table>';
		}
	$out.='
			<input type="hidden" name="notInDBArray" value="'.join(',',$notInDBArray).'">
		</form>
	';
		
	}else{

	// process area
		if($action=='renDir'){
			$newDir=stripslashes($_REQUEST['newDir']);
			if(ereg('/',$newDir)){
				header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&error='.$TEXT_ERROR_INVALID_DIRECTORY_NAME_CONST);
			}else{
				$newPath=str_replace($oldDir,$newDir,$path);
				exec('sudo -u root mv "'.$oldDir.'" "'.$newDir.'"');
				$out.='
				<script>
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($newPath).'&msg='.$TEXT_DIRECTORY_RENAMED_CONST.'";
					top.treeframe.location="index.php?section=leftMediaFilesSync&startPath='.urlencode($newPath).'";
				</script>';				
			}
		}

		// delete file from database
		if($action=='delfile'){
			$dfile=(int)@$_REQUEST['dfile'];
			$fileInfo=getFieldsAsArray('File','Path,Filename,IsDirectory',$mediadbADO,'WHERE PK_File='.$dfile);
			if($fileInfo['IsDirectory'][0]==0){
				// is file, delete all references
				delete_file_from_db($dfile,$mediadbADO);
			}else{
				// is directory, get all subdirectories and delete them recoursively
				delete_directory_from_db($fileInfo['Path'][0],$mediadbADO);
			}

			header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&msg='.$TEXT_FILE_WAS_DELETED_FROM_DATABASE_CONST);
			exit();
		}
		
		if($action=='delDir'){
			$newPath=getUpperLevel($path);
			exec('sudo -u root rm -rf "'.$path.'"');
			$out.='
				<script>
					self.location="index.php?section=mainMediaFilesSync&msg='.$TEXT_DIRECTORY_DELETED_CONST.'";
					top.treeframe.location="index.php?section=leftMediaFilesSync&startPath='.urlencode($newPath).'";
				</script>';	
		}

		if($action=='newDir'){
			$subDir=stripslashes($_REQUEST['subDir']);
			if(ereg('/',$subDir)){
				header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&error='.$TEXT_ERROR_INVALID_SUBDIRECTORY_NAME_CONST);
			}else{
				$newPath=$path.'/'.$subDir;
				exec('sudo -u root mkdir "'.$newPath.'"');
				$out.='
				<script>
					self.location="index.php?section=mainMediaFilesSync&path='.urlencode($newPath).'&msg='.$TEXT_SUBDIRECTORY_WAS_CREATED_CONST.'";
					top.treeframe.location="index.php?section=leftMediaFilesSync&startPath='.urlencode($newPath).'";
				</script>';				
			}
			
		}
		
		if($action=='resync'){
			header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&msg='.$TEXT_SYNCHRONIZE_COMMAND_SENT_CONST);			
			exit();
		}
		
		if($action=='update'){
			$notInDBArray=explode(',',@$_POST['notInDBArray']);
			foreach($notInDBArray AS $physicalkey){
				$filename=stripslashes(@$_POST['filename_'.$physicalkey]);
				$type=(int)@$_POST['type_'.$physicalkey];
				if($type!=0){
					$isDir=(is_dir(stripslashes($path).'/'.$filename))?1:0;
					$mediadbADO->Execute('INSERT INTO File (EK_MediaType, Path, Filename,IsDirectory) VALUES (?,?,?,?)',array($type,stripslashes($path),$filename,$isDir));
				}
			}
			header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($path).'&msg='.$TEXT_FILE_ADDED_TO_DATABASE_CONST);
		}
	}
	
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_MEDIA_FILES_SYNC_CONST);
	$output->setReloadLeftFrame(false);	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function getUpperLevel($path)
{
	if($path=='/')
		return $path;
	$upLevel=substr($path,0,-1);
	return substr($upLevel,0,strrpos($upLevel,'/'));
}

function delete_file_from_db($dfile,$mediadbADO){
	// delete records where file is foreign key
	$foreignTables=array('File_Attribute','Picture_File','PlaylistEntry');
	foreach($foreignTables AS $tablename){	
		$mediadbADO->Execute('DELETE FROM '.$tablename.' WHERE FK_File='.$dfile);
	}	
	$mediadbADO->Execute('DELETE FROM File WHERE PK_File='.$dfile);
}

function delete_directory_from_db($directoryPath,$mediadbADO){
	$mediadbADO->debug=true;
	$res=$mediadbADO->Execute("SELECT * FROM File WHERE Path LIKE '$directoryPath/%'");
	
	while($row=$res->FetchRow()){
		if($row['IsDirectory']==1){
			delete_directory_from_db($row['Path'],$mediadbADO);
		}
		delete_file_from_db($row['PK_File'],$mediadbADO);
	}
}
?>
