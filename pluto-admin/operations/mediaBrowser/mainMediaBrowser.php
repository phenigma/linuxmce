<?
function mainMediaBrowser($output,$mediadbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';

	$picsPerLine=6;
	
	if($action=='form'){
		if(isset($_POST['displayedTypes'])){
			$displayedTypes=@$_POST['displayedTypes'];
			$checkedTypes=array();
			$displayedTypesArray=explode(',',$displayedTypes);
			foreach($displayedTypesArray AS $value)
				if(isset($_POST['type_'.$value]))
					$checkedTypes[]=$value;
		}else 
			$checkedTypes=explode(',',@$_SESSION['checkedTypes']);
			
		if(@$_REQUEST['searchType']!=''){
			$searchString=$_REQUEST['searchType'].'%';
			unset($_SESSION['mediaSearchString']);
		}else
			$searchString='%'.@$_SESSION['mediaSearchString'].'%';
		
		if(count($checkedTypes)>0){
			$_SESSION['searchString']=$searchString;
			$_SESSION['checkedTypes']=join(',',$checkedTypes);
		}
		$checkedTypesArray=explode(',',@$_SESSION['checkedTypes']);
		$out.='
			<form action="index.php" method="POST" name="mainMediaBrowser" enctype="multipart/form-data">
				<input type="hidden" name="section" value="mainMediaBrowser">
				<input type="hidden" name="action" value="">
				<input type="hidden" name="searchType" value="">
				<input type="hidden" name="attributeID" value="">
		';
		$queryTypes='SELECT * FROM AttributeType ORDER BY Description ASC';
		$resTypes=$mediadbADO->Execute($queryTypes);
		$displayedTypes=array();
		while($rowTypes=$resTypes->FetchRow()){
			$displayedTypes[]=$rowTypes['PK_AttributeType'];
			$out.='<input type="checkbox" name="type_'.$rowTypes['PK_AttributeType'].'" value="1" '.((isset($_POST['type_'.$rowTypes['PK_AttributeType']])|| in_array($rowTypes['PK_AttributeType'],$checkedTypesArray))?'checked':'').'> '.$rowTypes['Description'].' ';
		}
		$out.='
			<input type="hidden" name="displayedTypes" value="'.join(',',$displayedTypes).'">
		</form>';
	
		if(isset($_SESSION['checkedTypes']) && $_SESSION['checkedTypes']!=''){
			$out.='<table>';
			if($_SESSION['searchString']!='none%'){
				$querySearch='
						SELECT Attribute.*, AttributeType.Description AS AttrType 
							FROM Attribute 
						INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
						WHERE (Name LIKE ? OR FirstName LIKE ?) AND FK_AttributeType IN ('.$_SESSION['checkedTypes'].') 
						ORDER BY Name ASC';
				$resSearch=$mediadbADO->Execute($querySearch,array($_SESSION['searchString'],$_SESSION['searchString']));	
			}
			else{
				$querySearch="
						SELECT Attribute.*, AttributeType.Description AS AttrType 
							FROM Attribute 
						INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
						WHERE (Name REGEXP '^[0-9|\"|\']' OR FirstName REGEXP '^[0-9|\"|\']') AND FK_AttributeType IN (".$_SESSION['checkedTypes'].") 
						ORDER BY Name ASC";
				$resSearch=$mediadbADO->Execute($querySearch);
			}
			if($resSearch->RecordCount()==0){
				$out.='
					<tr>
						<td colspan="3">No records.</td>
					</tr>
				';
			}
			while($rowSearch=$resSearch->FetchRow()){
				$out.='
						<tr>
							<td>'.$rowSearch['Name'].(($rowSearch['FirstName']!='')?', '.$rowSearch['FirstName']:'').'</td>
							<td>'.$rowSearch['AttrType'].'</td>
							<td><a href="#" onClick="document.mainMediaBrowser.action.value=\'properties\';document.mainMediaBrowser.attributeID.value='.$rowSearch['PK_Attribute'].';document.mainMediaBrowser.submit();">Properties</a></td>
						</tr>';
			}
			$out.='</table>';
		}else
			$out.='No attribute selected.';
	}elseif($action=='properties'){
		$attributeID=$_REQUEST['attributeID'];
		$queryAttribute='SELECT * FROM Attribute WHERE PK_Attribute=?';		
		$resAttribute=$mediadbADO->Execute($queryAttribute,$attributeID);
		$rowAttribute=$resAttribute->FetchRow();
		$attrType=$rowAttribute['FK_AttributeType'];
		if(isset($_REQUEST['picID'])){
			$toDelete=$_REQUEST['picID'];
			$deletePic='DELETE FROM Picture WHERE PK_Picture=?';
			$mediadbADO->Execute($deletePic,$toDelete);
			
			$deletePicAttribute='DELETE FROM Picture_Attribute WHERE FK_Picture=?';
			$mediadbADO->Execute($deletePicAttribute,$toDelete);
			
			unlink($GLOBALS['mediaPicsPath'].$toDelete.'.jpg');
			unlink($GLOBALS['mediaPicsPath'].$toDelete.'_tn.jpg');
		}
		$out.='
			<div class="err">'.((isset($_REQUEST['error']))?$_REQUEST['error']:'').'</div>
			<form action="index.php" method="POST" name="mainMediaBrowser" enctype="multipart/form-data">
				<input type="hidden" name="section" value="mainMediaBrowser">
				<input type="hidden" name="action" value="">
				<input type="hidden" name="searchType" value="">
				<input type="hidden" name="attributeID" value="'.$attributeID.'">
			<table border="0">
				<tr>
					<td colspan="6" align="left"><h3>Attribute</h3></td>
				</tr>		
				<tr>
					<td><B>Name:</B> </td>
					<td><input type="text" name="attrName" value="'.$rowAttribute['Name'].'"></td>
					<td><B>First Name:</B> </td>
					<td><input type="text" name="attrFirstName" value="'.$rowAttribute['FirstName'].'"></td>
					<td><B>Type:</B> </td>
					<td><select name="attrType">
							<option value="0"></option>';
			$queryTypes='SELECT * FROM AttributeType ORDER BY Description ASC';
			$resTypes=$mediadbADO->Execute($queryTypes);
			while($rowTypes=$resTypes->FetchRow()){
				$out.='<option value="'.$rowTypes['PK_AttributeType'].'" '.(($rowTypes['PK_AttributeType']==$rowAttribute['FK_AttributeType'])?'selected':'').'>'.$rowTypes['Description'].'</option>';
			}
			$out.='</select></td>
				</tr>
				<tr>
					<td colspan="6" align="center"><input type="button" name="update" value="Update" onclick="document.mainMediaBrowser.action.value=\'update\';document.mainMediaBrowser.submit();"></td>
				</tr>
				<tr>
					<td colspan="6" align="center">&nbsp;</td>
				</tr>
				<tr>
					<td valign="top" align="left" colspan="6">';
			$queryFiles='
				SELECT FK_File, FK_Attribute,FK_Type, Path, Filename,PK_File, Missing
				FROM File_Attribute
					INNER JOIN File ON File_Attribute.FK_File=PK_File
				WHERE FK_Attribute=?';
			$GLOBALS['attrType']=$attrType;
			$GLOBALS['attributeID']=$attributeID;
		$out.=multi_page($queryFiles, $attributeID,'index.php?section=mainMediaBrowser&attributeID='.$attributeID.'&action=properties', (isset($_GET['page_no']))?$_GET['page_no']-1:0, 20,$mediadbADO);
		
			$fileTypeSelect='<select name="newType">
				<option value="0">Please select</option>';
			$selectFileType='SELECT * FROM Type ORDER BY Description ASC';
			$resFileType=$mediadbADO->Execute($selectFileType);
			while($rowFileType=$resFileType->FetchRow()){
				$fileTypeSelect.='<option value="'.$rowFileType['PK_Type'].'">'.$rowFileType['Description'].'</option>';
			}
			$fileTypeSelect.='</select>';
			$out.='</table></td>
				</tr>
				<tr>
					<td><B>Add file</B></td>
					<td colspan="6">Name: <input type="text" name="newFile" value=""> Path: <input type="text" name="newPath" value=""> Type: '.$fileTypeSelect.' <input type="button" name="update" value="Add file" onclick="document.mainMediaBrowser.action.value=\'update\';document.mainMediaBrowser.submit();"></td>
				</tr>
				<tr>
					<td valign="top" align="left"><B>Pictures</B></td>
					<td valign="top" align="left" colspan="6"><table>
				<tr>';
			$queryPictures='
				SELECT * FROM Picture_Attribute
				INNER JOIN Picture ON Picture_Attribute.FK_Picture=PK_Picture
				WHERE FK_Attribute=?';
			$resPictures=$mediadbADO->Execute($queryPictures,$attributeID);
			$picsCount=0;
			while($rowPictures=$resPictures->FetchRow()){
				$picsCount++;
				$out.='
					<td style="background-color:#EEEEEE;" align="center"><a href="mediapics/'.$rowPictures['PK_Picture'].'.'.$rowPictures['Extension'].'" target="_blank"><img src="mediapics/'.$rowPictures['PK_Picture'].'_tn.'.$rowPictures['Extension'].'" border="0"></a> <br><a href="#" onClick="if(confirm(\'Are you sure you want to delete this picture?\'))self.location=\'index.php?section=mainMediaBrowser&attributeID='.$attributeID.'&action=properties&picID='.$rowPictures['PK_Picture'].'\';">Delete</a></td>
				';
				if($picsCount%$picsPerLine==0)
					$out.='</tr><tr>';
			}
			$out.='
					</tr>
				</table></td>
				</tr>
				<tr>
					<td><B>Add picture</B></td>
					<td colspan="6"> <input type="file" name="newPic" value=""> <input type="button" name="update" value="Add Picture" onclick="document.mainMediaBrowser.action.value=\'update\';document.mainMediaBrowser.submit();"></td>
				</tr>

			</table>
		</form>
		';
		
	}else{
		$name=$_POST['attrName'];
		$firstName=$_POST['attrFirstName'];
		$attrType=($_POST['attrType']!='0')?(int)$_POST['attrType']:NULL;
		$attributeID=(int)$_POST['attributeID'];
		
		if(!isset($_POST['merge'])){
			$queryExistingAttribute='
				SELECT Attribute.*, AttributeType.Description AS AttributeType
				FROM Attribute 
					INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				WHERE Name=? AND FK_AttributeType=? AND PK_Attribute!=?';
			$resExistingAttribute=$mediadbADO->Execute($queryExistingAttribute,array($name,$attrType,$attributeID));
			if($resExistingAttribute->RecordCount()>0){
				$rowExistingAttribute=$resExistingAttribute->FetchRow();
				$out.='
					<form action="index.php" method="POST" name="mainMediaBrowser" enctype="multipart/form-data">
						<input type="hidden" name="section" value="mainMediaBrowser">
						<input type="hidden" name="searchType" value="">
						<input type="hidden" name="action" value="update">
						<input type="hidden" name="attrName" value="'.$name.'">
						<input type="hidden" name="attrFirstName" value="'.$firstName.'">
						<input type="hidden" name="attrType" value="'.$attrType.'">
						<input type="hidden" name="attributeID" value="'.$attributeID.'">
						<input type="hidden" name="existingAttributeID" value="'.$rowExistingAttribute['PK_Attribute'].'">
				<table>
					<tr>
						<td height="40">
							The '.$rowExistingAttribute['AttributeType'].' <B>'.$name.'</B> already exists. Do you want to merge this one with it?
						</td>
					</tr>
					<tr>
						<td align="center">
							<input type="submit" name="merge" value="Yes, merge them">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" name="cancel" value="Cancel" onClick="self.location=\'index.php?section=mainMediaBrowser&attributeID='.$attributeID.'&action=properties\';"> 
						</td>
					</tr>
				
				</table>
					</form>
				';
				$dontRedirect=true;
			}else{
				$updateAttribute='UPDATE Attribute SET Name=?, FirstName=?, FK_AttributeType=? WHERE PK_Attribute=?';
				$mediadbADO->Execute($updateAttribute,array($name,$firstName,$attrType,$attributeID));
			}
				
		}else{
			$existingAttributeID=(int)$_POST['existingAttributeID'];

			// delete current attribute
			$deleteCurrentAttribute='DELETE FROM Attribute WHERE PK_Attribute=?';
			$mediadbADO->Execute($deleteCurrentAttribute,$attributeID);
			
			//	asign Files and Pictures from current attribute to existing one
			$updateFileAttribute='UPDATE File_Attribute SET FK_Attribute=? WHERE FK_Attribute=?';
			$mediadbADO->Execute($updateFileAttribute,array($existingAttributeID,$attributeID));
			
			$updatePictureAttribute='UPDATE Picture_Attribute SET FK_Attribute=? WHERE FK_Attribute=?';
			$mediadbADO->Execute($updatePictureAttribute,array($existingAttributeID,$attributeID));

			// set return attribute to existing one
			$attributeID=$existingAttributeID;

		}		
		
		$newFile=cleanString(@$_POST['newFile']);
		$newPath=cleanString(@$_POST['newPath']);
		$newType=(@$_POST['newPath']!='0')?@$_POST['newPath']:NULL;
		
		if(($newFile!='' && $newPath!='')){
			$insertFile='INSERT INTO File(Path, Filename, FK_Type) VALUES (?,?,?)';
			$mediadbADO->Execute($insertFile,array($newPath,$newFile,$newType));
			
			$insertID=$mediadbADO->Insert_ID();
			$insertFile_Attribute='INSERT INTO File_Attribute (FK_File, FK_Attribute) VALUES (?,?)';
			$mediadbADO->Execute($insertFile_Attribute,array($insertID,$attributeID));
		}
		
		if(isset($_FILES['newPic']) && $_FILES['newPic']['name']!=''){
			$picExtension=str_replace('.','',strtolower(strrchr($_FILES['newPic']['name'],".")));
			$insertPicture='INSERT INTO Picture (Extension) VALUES (?)';
			$mediadbADO->Execute($insertPicture,$picExtension);
			$insertID=$mediadbADO->Insert_ID();
			$newPicName=$insertID.'.'.$picExtension;
			
			$error='';
			if(($_FILES['newPic']['type']!="image/jpg") && ($_FILES['newPic']['type']!="image/pjpeg") && ($_FILES['newPic']['type']!="image/jpeg")){
				$error='The file is not a jpg file';
			}
			elseif(move_uploaded_file($_FILES['newPic']['tmp_name'],$GLOBALS['mediaPicsPath'].$newPicName)){
				// create thumbnail
				$resizeFlag=resizeImage($GLOBALS['mediaPicsPath'].$newPicName, $GLOBALS['mediaPicsPath'].$insertID.'_tn.'.$picExtension, 100, 100);
				// update database
				$insertPictureAttribute='INSERT INTO Picture_Attribute (FK_Attribute, FK_Picture) VALUES (?,?)';
				$mediadbADO->Execute($insertPictureAttribute,array($attributeID,$insertID));
			}else{
				//upload fail, prompt error message
				$deletePicture='DELETE FROM Picture WHERE PK_Picture=?';
				$mediadbADO->Execute($deletePicture,$insertID);
				$error='Upload fail, check the rights for /mediapics directory.';
			}
			
		}
		if(!isset($dontRedirect))
			$out.='
				<script>
					self.location=\'index.php?section=mainMediaBrowser&attributeID='.$attributeID.'&action=properties'.((isset($error))?'&error='.$error:'').'\';
				</script>
				';
		
	}
	
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}