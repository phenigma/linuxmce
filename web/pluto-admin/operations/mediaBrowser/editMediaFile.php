<?
function editMediaFile($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$fileID=$_REQUEST['fileID'];
	$picsPerLine=6;	
	
	$scriptInHead='
	<script>
	function windowOpen(locationA,attributes) 
	{
		window.open(locationA,\'\',attributes);
	}

	function searchList()
	{
		fragment=document.editMediaFile.newAttributeName.value.toLowerCase();
		for(i=0;i<document.editMediaFile.existingAttributes.length-1;i++){
			listItemName=document.editMediaFile.existingAttributes[i].text.toLowerCase();	
			if(listItemName.substr(0,fragment.length)==fragment){
				document.editMediaFile.existingAttributes[i].selected=true;
				break;
			}else{
				document.editMediaFile.existingAttributes[i].selected=false;
			}
		}
	
	}

	function setAttributeName()
	{
		document.editMediaFile.newAttributeName.value=document.editMediaFile.existingAttributes[document.editMediaFile.existingAttributes.selectedIndex].text
	}
	
	function syncPath(path)
	{
		top.treeframe.location=\'index.php?section=leftMediaFilesSync&startPath=\'+escape(path);
		self.location=\'index.php?section=mainMediaFilesSync&path=\'+escape(path);
	}	
	</script>
	';
	
	if($action=='form'){
		
		$out.='
			<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
			<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
			<form action="index.php" method="POST" name="editMediaFile" enctype="multipart/form-data">
				<input type="hidden" name="section" value="editMediaFile">
				<input type="hidden" name="action" value="update">
				<input type="hidden" name="fileID" value="'.$fileID.'">
		';
		$resFile=$mediadbADO->Execute('SELECT * FROM File WHERE PK_File=?',$fileID);
		$rowFile=$resFile->FetchRow();
		
		$queryAttrTypes='
			SELECT * 
			FROM AttributeType 
			INNER JOIN MediaType_AttributeType ON FK_AttributeType=PK_AttributeType
			WHERE EK_MediaType=?
			ORDER BY Description ASC';
		$resAttrTypes=$mediadbADO->Execute($queryAttrTypes,$rowFile['EK_MediaType']);
		$attributeTypes=array();
		while($rowTypes=$resAttrTypes->FetchRow()){
			$attributeTypes[$rowTypes['PK_AttributeType']]=$rowTypes['Description'];
		}

		$out.='
		<h4>'.$TEXT_EDIT_MEDIA_FILE_CONST.'</h4>
		<a href="javascript:syncPath(\''.$rowFile['Path'].'\')">Back</a>
		<table border="0" cellspacing="0" cellpadding="3">
			<tr bgColor="#EEEEEE">
				<td><B>'.$TEXT_FILE_CONST.':</B></td>
				<td><input type="text" name="filename" value="'.$rowFile['Filename'].'" size="55"></td>
			</tr>
			<tr>
				<td>&nbsp;</td>
				<td align="right"><input type="button" class="button" name="delete" value="'.$TEXT_DELETE_FILE_CONST.'" onClick="if(confirm(\''.$TEXT_DELETE_FILE_CONFIRMATION_CONST.'\')){document.editMediaFile.action.value=\'del\';document.editMediaFile.submit();}"> <input type="button" class="button" name="moveFile" value="Move file to other directory" onClick="windowOpen(\'index.php?section=moveFile&fileID='.$fileID.'&filePath='.urlencode(stripslashes($rowFile['Path']).'/'.$rowFile['Filename']).'\',\'width=500,height=400,toolbars=true,scrollbars=true,resizable=1\');"></td>
			</tr>
			<tr bgcolor="#EBEFF9">
				<td><B>'.$TEXT_LOCATION_CONST.':</B></td>
				<td><input type="text" name="Path" value="'.stripslashes($rowFile['Path']).'" size="55">'.((file_exists($rowFile['Path'].'/'.$rowFile['Filename'])?'<img src=include/images/sync.gif align=middle border=0>':'<img src=include/images/db.gif align=middle border=0>')).'</td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<td><B>'.$TEXT_TYPE_CONST.':</B></td>
				<td><select name="type">';
			$resType=$dbADO->Execute('SELECT * FROM MediaType ORDER BY Description ASC');
			while($rowType=$resType->FetchRow()){
				$out.='<option value="'.$rowType['PK_MediaType'].'" '.(($rowType['PK_MediaType']==$rowFile['EK_MediaType'])?'selected':'').'>'.$rowType['Description'].'</option>';
			}
			$out.='
				</select></td>
			</tr>
			<tr bgcolor="#EBEFF9">
				<td valign="top"><B>'.$TEXT_ATTRIBUTES_CONST.':</B></td>
				<td><table width="100%">';
			$queryAttributes='
				SELECT Attribute.*, AttributeType.Description 
				FROM Attribute
				INNER JOIN File_Attribute ON FK_Attribute=PK_Attribute
				INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				WHERE FK_File=?
				ORDER BY Description ASC';
			$resAttributes=$mediadbADO->Execute($queryAttributes,$fileID);
			$pos=0;
			while($rowAttributes=$resAttributes->FetchRow()){
				$pos++;
				$out.='
						<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#FFFFFF').'">
							<td><b>'.$rowAttributes['Description'].'</b></td>
							<td><a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties">'.$rowAttributes['Name'].'</a></td>
							<td align="center"><a href="#" onClick="if(confirm(\''.$TEXT_DELETE_ATTRIBUTE_FROM_FILE_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'&action=delete&dAtr='.$rowAttributes['PK_Attribute'].'&dpath='.stripslashes($rowFile['Path']).'\'">'.$TEXT_REMOVE_CONST.'</a></td>
						</tr>';
			}
			$out.='
				</table></td>
			</tr>			
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"> <input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'">
				<input type="hidden" name="oldPath" value="'.$rowFile['Path'].'">
				<input type="hidden" name="oldFilename" value="'.$rowFile['Filename'].'">
				</td>
			</tr>			
			<tr>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td valign="top" align="left"><B>'.$TEXT_PICTURES_CONST.'</B></td>
				<td valign="top" align="left" colspan="6"><table>
			<tr>';
			$queryPictures='
				SELECT * FROM Picture_File
				INNER JOIN Picture ON Picture_File.FK_Picture=PK_Picture
				WHERE FK_File=?';
			$resPictures=$mediadbADO->Execute($queryPictures,$fileID);
			$picsCount=0;
			while($rowPictures=$resPictures->FetchRow()){
				$picsCount++;
				$out.='
					<td style="background-color:#EEEEEE;" align="center"><a href="mediapics/'.$rowPictures['PK_Picture'].'.'.$rowPictures['Extension'].'" target="_blank"><img src="mediapics/'.$rowPictures['PK_Picture'].'_tn.'.$rowPictures['Extension'].'" border="0"></a> <br><a href="#" onClick="if(confirm(\'Are you sure you want to delete this picture?\'))self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'&action=properties&picID='.$rowPictures['PK_Picture'].'\';">'.$TEXT_DELETE_CONST.'</a></td>
				';
				if($picsCount%$picsPerLine==0)
					$out.='</tr><tr>';
			}
			$out.='
					</tr>
				</table></td>
			</tr>
			<tr>
				<td><B>'.$TEXT_ADD_PICTURE_CONST.'</B><br><em>'.$TEXT_JPG_ONLY_CONST.'</em></td>
				<td colspan="6"> <input type="file" name="newPic" value=""> <input type="submit" class="button" name="addPic" value="'.$TEXT_ADD_PICTURE_CONST.'"></td>
			</tr>
			<tr>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>
			
			<tr>
				<td><B>'.$TEXT_ADD_ATTRIBUTE_CONST.':</B><br><select name="newAttributeType" onChange="document.editMediaFile.action.value=\'form\';document.editMediaFile.submit();">
					<option value="0">- '.$TEXT_PLEASE_SELECT_CONST.' -</option>';
			foreach($attributeTypes AS $attributeID=>$attributeName){
				$out.='<option value="'.$attributeID.'" '.(($attributeID==@$_POST['newAttributeType'])?'selected':'').'>'.$attributeName.'</option>';
			}
			$out.='</select></td>
				<td><B>'.$TEXT_ATTRIBUTE_NAME_CONST.' *</B><br><input type="text" name="newAttributeName" value=""></td>
			</tr>';
			if(isset($_POST['newAttributeType']) && $_POST['newAttributeType']!='0'){
				$newAttributeType=(int)$_POST['newAttributeType'];
				$resAttributesByType=$mediadbADO->Execute('SELECT * FROM Attribute WHERE FK_AttributeType=? ORDER BY Name ASC',$newAttributeType);
				$out.='
				<tr>
					<td>&nbsp;</td>
					<td>';
				if($resAttributesByType->RecordCount()>0){
					$out.='
				<select name="existingAttributes" size="20" onClick="setAttributeName();" onChange="setAttributeName();">';
				while($rowAttributesByType=$resAttributesByType->FetchRow()){
					$out.='<option value="'.$rowAttributesByType['PK_Attribute'].'">'.$rowAttributesByType['Name'].'</option>';
				}
				$out.='
					</select>';
				}
				$out.='
				<input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
				</tr>';
			}
			$out.='
		</table>';
		$out.='
		</form>';
		if(isset($_POST['newAttributeType']) && $_POST['newAttributeType']!='0'){
			$out.='
			<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
			<script>
			 	var frmvalidator = new formValidator("editMediaFile");			
	 			frmvalidator.addValidation("newAttributeName","req","'.$TEXT_ATTRIBUTE_NAME_VALIDATION_CONST.'");
			</script>		
			';
		}	
	}else{
	// process area
		if(isset($_REQUEST['picID'])){
			$toDelete=$_REQUEST['picID'];
			$deletePic='DELETE FROM Picture WHERE PK_Picture=?';
			$mediadbADO->Execute($deletePic,$toDelete);
			
			$deletePicAttribute='DELETE FROM Picture_Attribute WHERE FK_Picture=?';
			$mediadbADO->Execute($deletePicAttribute,$toDelete);
			
			$deletePicFile='DELETE FROM Picture_File WHERE FK_Picture=?';
			$mediadbADO->Execute($deletePicFile,$toDelete);
			
			@unlink($GLOBALS['mediaPicsPath'].$toDelete.'.jpg');
			@unlink($GLOBALS['mediaPicsPath'].$toDelete.'_tn.jpg');
		}

		if(isset($_POST['add'])){
			$newAttributeType=$_POST['newAttributeType'];
			$newAttributeName=cleanString($_POST['newAttributeName']);
			$existingAttributes=(int)@$_POST['existingAttributes'];
			if($existingAttributes!=0){
				$resExistingAttribute=$mediadbADO->Execute('SELECT Name FROM Attribute WHERE PK_Attribute=?',$existingAttributes);
				$rowExistingAttribute=$resExistingAttribute->FetchRow();
				if($rowExistingAttribute['Name']!=$newAttributeName){
					
					// new attribute, insert in Attribute and File_Attribute tables
					
					
					$AttributeAdded=addAttribute($newAttributeType,$newAttributeName,$fileID,$dbADO);
					$isAdded=(ereg('RESP: OK',$AttributeAdded))?true:false;
					
					if($isAdded!==true){
						header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&error='.$TEXT_ERROR_ATTRIBUTE_NOT_ADDED_CONST.': '.urlencode(nl2br($AttributeAdded)));
						exit();
					}
					
				}else{
					$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_File, FK_Attribute) VALUES (?,?)',array($fileID,$existingAttributes));
				}
			}else{
				$AttributeAdded=addAttribute($newAttributeType,$newAttributeName,$fileID,$dbADO);
				$isAdded=(ereg('RESP: OK',$AttributeAdded))?true:false;
				if($isAdded!==true){
					header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&error='.$TEXT_ERROR_ATTRIBUTE_NOT_ADDED_CONST.': '.urlencode(nl2br($AttributeAdded)));
					exit();
				}
			}
			
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_FILE_ATTRIBUTE_ADDED_CONST.':<br>'.urlencode(nl2br(@$AttributeAdded)));
			exit();
		}
		
		if(isset($_REQUEST['dAtr'])){
			$deleteAttribute=$_REQUEST['dAtr'];
			$mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_File=? AND FK_Attribute=?',array($fileID,$deleteAttribute));
			
			$dpath=$_REQUEST['dpath'];
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -d "'.$dpath.'"';
			exec($cmd);
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_ATTRIBUTE_DELETED_FROM_FILE_CONST.': '.$cmd);	
			exit();		
		}
	
		$path=stripslashes(@$_POST['Path']);
		$fileName=@$_POST['filename'];
		$newFilePath=$path.'/'.$fileName;

		$oldPath=stripslashes(@$_POST['oldPath']);
		$oldFilename=stripslashes(@$_POST['oldFilename']);
		$oldFilePath=$oldPath.'/'.$oldFilename;
		
		if($action=='del'){
			// delete physical file
			$cmd='sudo -u root rm -f "'.$oldFilePath.'"';
			exec($cmd);
			$mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_File=?',$fileID);
			$mediadbADO->Execute('DELETE FROM Picture_File WHERE FK_File=?',$fileID);
			$mediadbADO->Execute('DELETE FROM File WHERE PK_File=?',$fileID);
			
			exec('sudo -u root /usr/pluto/bin/UpdateMedia -d "'.$oldPath.'"');
			header('Location: index.php?section=mainMediaFilesSync&path='.urlencode($oldPath).'&msg='.$TEXT_MEDIA_FILE_DELETED_CONST);			
			exit();

		}
		
		
		if(isset($_POST['update'])){
			$type=(int)$_POST['type'];
						
			if(file_exists($oldFilePath)){
				if($path==$oldPath){
					if($fileName!=$oldFilename)
						rename($oldFilePath,$newFilePath);
				}
				else{
					copy($oldFilePath,$newFilePath);
					exec('sudo -u root rm -f '.$oldFilePath);
				}
			}
			$mediadbADO->Execute('UPDATE File SET Filename=?, Path=?, EK_MediaType=? WHERE PK_File=?',array($fileName,$path,$type,$fileID));
			
			$cmd='sudo -u root /usr/pluto/bin/UpdateMedia -d "'.$path.'"';
			exec($cmd);
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_MEDIA_FILE_UPDATED_CONST.': '.$cmd);			
			exit();
		}
		
		if(isset($_REQUEST['addPic']) && isset($_FILES['newPic']) && $_FILES['newPic']['name']!=''){
			$picExtension=str_replace('.','',strtolower(strrchr($_FILES['newPic']['name'],".")));
			$picExtension=($picExtension=='jpeg')?'jpg':$picExtension;
			
			$insertPicture='INSERT INTO Picture (Extension) VALUES (?)';
			$mediadbADO->Execute($insertPicture,$picExtension);
			$insertID=$mediadbADO->Insert_ID();
			$newPicName=$insertID.'.'.$picExtension;
			
			$error='';
			if(($_FILES['newPic']['type']!="image/jpg") && ($_FILES['newPic']['type']!="image/pjpeg") && ($_FILES['newPic']['type']!="image/jpeg")){
				$error=$TEXT_ERROR_NOT_JPEG_CONST;
			}
			elseif(move_uploaded_file($_FILES['newPic']['tmp_name'],$GLOBALS['mediaPicsPath'].$newPicName)){
				// create thumbnail
				$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$insertID.'_tn.'.$picExtension, 100, 100);
				// update database
				$insertPictureAttribute='INSERT INTO Picture_File (FK_File, FK_Picture) VALUES (?,?)';
				$mediadbADO->Execute($insertPictureAttribute,array($fileID,$insertID));
			}else{
				//upload fail, prompt error message
				$deletePicture='DELETE FROM Picture WHERE PK_Picture=?';
				$mediadbADO->Execute($deletePicture,$insertID);
				$error=$TEXT_ERROR_UPLOAD_FAILS_PERMISIONS_CONST.' '.$GLOBALS['mediaPicsPath'];
			}
			if($error!=''){
				header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&error='.$error);			
				exit();
			}else{
				exec('sudo -u root /usr/pluto/bin/UpdateMedia -d "'.$path.'"');
				header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_PICTURE_UPLOADED_CONST);			
				exit();
			}
		}
		
		header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg='.$TEXT_MEDIA_FILE_UPDATED_CONST);			
	}
	$output->setReloadLeftFrame(false);
	$output->setScriptInHead($scriptInHead);	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function addAttribute($newAttributeType,$newAttributeName,$fileID,$dbADO){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
		
	$mediaPlugin=getMediaPluginID($_SESSION['installationID'],$dbADO);
	if(is_null($mediaPlugin)){
		header("Location: index.php?section=editMediaFile&fileID=$fileID&error=$TEXT_MEDIA_PLUGIN_NOT_FOUND_CONST");
		exit();
	}

	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -r 0 '.$mediaPlugin.' 1 391 122 '.$newAttributeType.' 145 '.$fileID.' 5 "'.$newAttributeName.'"';
	exec($cmd,$ret);
	$response=join('<br>',$ret);
	$suffix=(ereg('RESP: OK',$response))?'RESP: OK':'';
	
	return $cmd.'<br>'.$TEXT_RESPONSE_CONST.': '.$suffix;
}
