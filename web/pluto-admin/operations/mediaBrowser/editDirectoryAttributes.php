<?
function editDirectoryAttributes($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$fileID=$_REQUEST['fileID'];
	$dirData=getRows('File','File.*',$mediadbADO,'WHERE PK_File=\''.$fileID.'\'');
	$filesArray=getAssocArray('File','PK_File','Filename',$mediadbADO,'WHERE Path LIKE \''.@$dirData[0]['Path'].'/'.@$dirData[0]['Filename'].'\' AND Missing=0 AND PK_File!=\''.$fileID.'\' AND IsDirectory=0 ORDER BY Filename ASC');
	
	if($action=='form'){
		
	
	$scriptInHead='
	<script>

	function setAll(bylink){
		if(bylink==1){
			editDirectoryAttributes.all.checked=(editDirectoryAttributes.all.checked)?false:true;
		}
		val=editDirectoryAttributes.all.checked;

		for (i = 0; i < editDirectoryAttributes.elements.length; i++)
		   {
			tmpName=editDirectoryAttributes.elements[i].name;
		     if (editDirectoryAttributes.elements[i].type == "checkbox" && tmpName.indexOf("file_")!=-1)
		     {
		         editDirectoryAttributes.elements[i].checked = val;
		     }
		   }
		}	
	</script>
	';
		
		$out='
			<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
			<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
			
		<table width="100%">
			<tr>
				<td><a href="javascript:syncPath(\''.$rowFile['Path'].'\')">Back</a></td>
				<td align="right">'.quick_search_box().'</td>
			<tr>
		</table>
					
			<form action="index.php" method="POST" name="editDirectoryAttributes" enctype="multipart/form-data">
				<input type="hidden" name="section" value="editDirectoryAttributes">
				<input type="hidden" name="action" value="update">
				<input type="hidden" name="fileID" value="'.$fileID.'">
		';
		if(count($dirData)==0){
			$out.='<span class="err">'.$TEXT_DIRECTORY_NOT_SPECIFIED_CONST.'</span>';
		}else{
			if(count($filesArray)==0){
				$out.='<span class="err">'.$TEXT_NO_FILES_ON_DIRECTORY_CONST.'</span>';
			}else{
				$out.='<table>';
				foreach ($filesArray AS $id=>$filename){
					$out.='
					<tr>
						<td><input type="checkbox" name="file_'.$id.'" value="1" checked></td>
						<td><a href="index.php?section=editMediaFile&fileID='.$id.'">'.$filename.'</a></td>
					</tr>
					';
				}
				$out.='
					<tr>
						<td colspan="4"><hr width="60%" align="left"></td>
					</tr>				
					<tr>
						<td><input type="checkbox" value="1" name="all" onClick="setAll(0);" checked></td>
						<td colspan="3"><a href="javascript:setAll(1);">Select/unselect all</a> </td>
					</tr>
					<tr>
						<td colspan="4">
						<fieldset><legend><B>'.$TEXT_ADD_PICTURE_CONST.'</B><em>'.$TEXT_JPG_ONLY_CONST.'</em></legend>
							<table>
							<tr>
								<td>'.$TEXT_FILE_CONST.'</td>
								<td><input type="file" name="newPic" value=""></td>
							</tr>
							<tr>
								<td>'.$TEXT_URL_CONST.'</td>
								<td><input type="text" name="newUrl" value=""></td>
							</tr>						
							<tr>
								<td><input type="checkbox" value="1" name="replace_pics"></td>
								<td>'.$TEXT_REPLACE_EXISTING_PICS_CONST.'</td>
							</tr>							
							<tr>
								<td></td>
								<td><input type="submit" class="button" name="addPic" value="'.$TEXT_ADD_PICTURE_CONST.'"></td>
							</tr>							
						</table>
						</fieldset>
						</td>
					</tr>				
					
			
									
				</table>';
			}			
		}
		$out.='
		</form>';
	}else{
	// process area
	
		if(isset($_REQUEST['addPic'])){
			if(isset($_FILES['newPic']) && $_FILES['newPic']['name']!=''){
				$newUrl=cleanString($_POST['newUrl']);
				$picExtension=str_replace('.','',strtolower(strrchr($_FILES['newPic']['name'],".")));
				$picExtension=($picExtension=='jpeg')?'jpg':$picExtension;
				
				$mediadbADO->Execute('INSERT INTO Picture (Extension,URL) VALUES (?,?)',array($picExtension,$newUrl));
				$insertID=$mediadbADO->Insert_ID();
				$newPicName=$insertID.'.'.$picExtension;
				
				$error='';
				if(($_FILES['newPic']['type']!="image/jpg") && ($_FILES['newPic']['type']!="image/pjpeg") && ($_FILES['newPic']['type']!="image/jpeg")){
					$error=$TEXT_ERROR_NOT_JPEG_CONST;
				}
				elseif(move_uploaded_file($_FILES['newPic']['tmp_name'],$GLOBALS['mediaPicsPath'].$newPicName)){
					// create thumbnail
					resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$insertID.'_tn.'.$picExtension, 256, 256);
					
					// update database
					$replace_pics=(int)@$_POST['replace_pics'];
					foreach ($filesArray AS $id=>$filename){
						if((int)@$_POST['file_'.$id]==1){
							if($replace_pics==1){
								// delete all existing pics for selected files
								$picsArray=getFields('Picture_File','FK_Picture',$mediadbADO,'WHERE FK_File='.$id);
								for($i=0;$i<count($picsArray);$i++){
									delete_media_pic($picsArray[$i]['FK_Picture'],$mediaADO);
								}
							}
							$mediadbADO->Execute('INSERT INTO Picture_File (FK_File, FK_Picture) VALUES (?,?)',array($id,$insertID));						
						}						
					}
				}else{
					//upload fail, prompt error message
					$mediadbADO->Execute('DELETE FROM Picture WHERE PK_Picture=?',$insertID);
					$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
				}
			}
		}
		
		header('Location: index.php?section=editDirectoryAttributes&fileID='.$fileID.'&msg='.$TEXT_MEDIA_FILE_UPDATED_CONST);			
		exit();
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_EDIT_ATTRIBUTES_FOR_FILES_IN_DIRECTORY_CONST);
	$output->setReloadLeftFrame(false);
	$output->setScriptInHead($scriptInHead);	
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_EDIT_ATTRIBUTES_FOR_FILES_IN_DIRECTORY_CONST=>'index.php?section=editDirectoryAttributes&fileID='.$fileID));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>