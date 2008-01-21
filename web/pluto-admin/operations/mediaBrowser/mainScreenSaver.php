<?
function mainScreenSaver($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$path = (isset($_REQUEST['path']) && $_REQUEST['path']!='')?cleanString($_REQUEST['path']):'';
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
	$records_per_page=50;
	$screenSaverAttribute=getScreenSaverAttribute($mediadbADO);
	
	if($action=='form'){
		$flickerEnabled=flickrStatus();
		$flicker_enable_disable_text=($flickerEnabled==1)?$TEXT_DISABLE_FLICKR_CONST:$TEXT_ENABLE_FLICKR_CONST;
		
		if($path!=''){
			$physicalFiles=grabFiles($path,'');
			
			$out.='
			<script>
			function windowOpen(locationA,attributes) 
			{
				window.open(locationA,\'\',attributes);
			}
			

			function syncPath(path)
			{
				top.treeframe.location=\'index.php?section=leftScreenSaver&startPath=\'+escape(path);
				self.location=\'index.php?section=mainScreenSaver&path=\'+escape(path);
			}
		function set_checkboxes()
		{
		   val=document.mainScreenSaver.sel_all.checked;
		   for (i = 0; i < mainScreenSaver.elements.length; i++)
		   {
			tmpName=mainScreenSaver.elements[i].name;
		     if (mainScreenSaver.elements[i].type == "checkbox")
		     {
		         mainScreenSaver.elements[i].checked = val;
		     }
		   }
		}			
			</script>
			
			<a href="javascript:syncPath(\''.substr($path,0,strrpos($path,'/')).'\')">Up one level</a>
			
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div><br>
			<div align="center" class="err"><B>'.@$_REQUEST['error'].'</B></div><br>
			<form action="index.php" method="POST" name="mainScreenSaver">
			<input type="hidden" name="section" value="mainScreenSaver">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="path" value="'.$path.'">
			';
			
			$out.='
				<input type="checkbox" name="flikr" value="1" '.(($flickerEnabled==1)?'checked':'').' onClick="document.mainScreenSaver.submit();"> '.$flicker_enable_disable_text.'
				<table cellpading="0" cellspacing="0">
					<tr>
						<td>'.getScreensaverFiles($path,$screenSaverAttribute,$mediadbADO,$page,$records_per_page).'</td>
					</tr>';
				$out.='		
				</table>';

		}
	$out.='
			
		</form>
	';

	}else{
		$flickr=(int)@$_REQUEST['flikr'];
		$parm=($flickr==1)?'-enable':'-disable';
		flickrStatus($parm);
		
		$displayedFiles=cleanString(@$_POST['displayedFiles']);
		if($displayedFiles!=''){
			$mediadbADO->Execute('DELETE FROM File_Attribute INNER JOIN FKAttribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=30 AND FK_File IN ('.$displayedFiles.')',array($screenSaverAttribute));
			$displayedFilesArray=explode(',',$displayedFiles);
			foreach ($displayedFilesArray AS $fileID){
				if((int)@$_POST['file_'.$fileID]==1){
					$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_Attribute,FK_File) VALUES (?,?)',array($screenSaverAttribute,$fileID));
				}
			}
		}
				
		header('Location: index.php?section=mainScreenSaver&path='.urlencode($path).'&msg='.cleanString($TEXT_SCREENSAVER_UPDATED_CONST));
		exit();
		
	}
	
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_SCREEN_SAVER_CONST);
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


function getScreensaverFiles($path,$screenSaverAttribute,$mediadbADO,$page,$records_per_page){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/screenSaver.lang.php');

	$query='SELECT PK_File,Filename,Path,FK_Attribute FROM File LEFT JOIN File_Attribute ON FK_File=PK_File AND FK_Attribute=? WHERE EK_MediaType IN (7,26) AND IsDirectory=0 AND Missing=0 AND Path=?';
	$res=$mediadbADO->Execute($query,array($screenSaverAttribute,$path));
	$records=$res->RecordCount();
	
	$noPages=ceil($records/$records_per_page);
	$start=($page-1)*$records_per_page;
	$end=$page*$records_per_page-1;
	$end=($end>$records)?$records:$end;	
	
	$pageLinks='Pages: ';
	for($i=1;$i<=$noPages;$i++){
		$pageLinks.=($i==$page)?' '.$i:' <a href="'.$_SERVER['PHP_SELF'].'?'.str_replace('&page='.$page,'',$_SERVER['QUERY_STRING']).'&page='.$i.'">'.$i.'</a>';
	}	
	
	$res=$mediadbADO->Execute($query.' LIMIT '.$start.','.$records_per_page,array($screenSaverAttribute,$path));
	if($res->RecordCount()==0){
		return $TEXT_NO_IMAGES_IN_DIRECTORY_CONST;
	}
	$out='
		<table celspacing="0" cellpadding="3" align="center" width="650">
			<tr class="tablehead">
				<td width="20">&nbsp;</td>
				<td width="20">&nbsp;</td>
				<td><B>'.$TEXT_IMAGE_CONST.'</B></td>
			</tr>';
	$pos=0;
	$displayedFiles=array();
	while($row=$res->FetchRow()){
		$displayedFiles[]=$row['PK_File'];
		$pos++;
		$class=($pos%2==0)?'alternate_back':'';
		$picUrl='include/image.php?imagepath='.$row['Path'].'/'.$row['Filename'];
		$filePic=(!file_exists($row['Path'].'/'.$row['Filename'].'.tnj'))?'&nbsp;':'<a href="'.$picUrl.'" target="_blank"><img src="include/image.php?imagepath='.$row['Path'].'/'.$row['Filename'].'.tnj" border="0"></a>';
		$out.='
			<tr class="'.$class.'">
				<td><input type="checkbox" name="file_'.$row['PK_File'].'" value="1" '.((is_null($row['FK_Attribute']))?'':'checked').'></td>
				<td>'.$filePic.'</td>
				<td><a href="'.$picUrl.'" target="_blank">'.$row['Filename'].'</a></td>
			</tr>		
		';
	}
	$out.='
			<tr>
				<td colspan="3"><input type="checkbox" name="sel_all" value="1" onClick="set_checkboxes();"> Select/unselect all<br>
				<input type="submit" class="button" name="save" value="Save">	
			</td>
			</tr>
			<tr>
				<td colspan="3" align="right">'.$pageLinks.'</td>
			</tr>		
	
	</table>
	<input type="hidden" name="displayedFiles" value="'.join(',',$displayedFiles).'">';
	
	return $out;
}

function getScreenSaverAttribute($mediadbADO){
	$res=$mediadbADO->Execute('SELECT PK_Attribute,Name FROM Attribute WHERE FK_AttributeType=30');
	if($res->RecordCount()==0){
		$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) SELECT PK_AttributeType,\'*\' FROM AttributeType WHERE PK_AttributeType=30');
		return $mediadbADO->Insert_id();
	}
	$attribute=0;
	while($row=$res->FetchRow()){
		if($attribute==0 || $row['Name']=='*'){
			$attribute=$row['PK_Attribute'];
		}
	}
	
	return $attribute;
}

function flickrStatus($param='-status'){
	$cmd='sudo -u root /usr/pluto/bin/flickr_status.sh '.$param;
	
	return trim(exec_batch_command($cmd,1));
}
?>