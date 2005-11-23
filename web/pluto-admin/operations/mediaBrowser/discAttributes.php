<?
function discAttributes($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/discAttributes.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$slot=$_REQUEST['slot'];
	$resSlot=$mediadbADO->Execute('SELECT * FROM Disc WHERE Slot=?',$slot);
	if($resSlot->RecordCount()==0){
		error_redirect($TEXT_ERROR_DATABASE_RECORD_NOT_FOUND_CONST,'index.php?section=discAttributes&slot='.$slot);
	}
	$rowSlot=$resSlot->FetchRow();
	$disc=$rowSlot['PK_Disc'];

	
	
	if($action=='form'){
		$attributeTypes=getAssocArray('AttributeType','PK_AttributeType','Description',$mediadbADO,'INNER JOIN MediaType_AttributeType ON FK_AttributeType=PK_AttributeType WHERE EK_MediaType='.$rowSlot['EK_MediaType'],'ORDER BY Description ASC');
		
		$out.='
			<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
			<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
			<form action="index.php" method="POST" name="discAttributes" enctype="multipart/form-data">
				<input type="hidden" name="section" value="discAttributes">
				<input type="hidden" name="action" value="update">
				<input type="hidden" name="slot" value="'.$slot.'">
		
		';
		$mediaTypes=getAssocArray('MediaType','PK_MediaType','Description',$dbADO,'','ORDER BY Description ASC');

		$out.='
		<h4>'.$TEXT_DISC_ATTRIBUTES_CONST.'</h4>
		<a href="index.php?section=powerFile">Back to Power File</a>
		<table border="0" cellspacing="0" cellpadding="3">
			<tr bgColor="#EEEEEE">
				<td><B>'.$TEXT_DISC_ID_CONST.':</B></td>
				<td><input type="text" name="disc_id" value="'.$rowSlot['ID'].'" size="55"></td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<td><B>'.$TEXT_TYPE_CONST.':</B></td>
				<td>'.pulldownFromArray($mediaTypes,'type',$rowSlot['EK_MediaType']).'</td>
			</tr>
			<tr bgcolor="#EBEFF9">
				<td valign="top"><B>'.$TEXT_DISC_ATTRIBUTES_CONST.':</B></td>
				<td><table width="100%">';
			$queryAttributes='
				SELECT Attribute.*, AttributeType.Description,Disc_Attribute.Track 
				FROM Attribute
				INNER JOIN Disc_Attribute ON FK_Attribute=PK_Attribute
				INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
				WHERE FK_Disc=?
				ORDER BY Track ASC,Description ASC';
			$resAttributes=$mediadbADO->Execute($queryAttributes,$rowSlot['PK_Disc']);
			$pos=0;
			while($rowAttributes=$resAttributes->FetchRow()){
				$pos++;
				$out.='
						<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#FFFFFF').'">
							<td><b>'.$rowAttributes['Track'].'</b></td>
							<td><b>'.$rowAttributes['Description'].'</b></td>
							<td><a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties">'.$rowAttributes['Name'].'</a></td>
							<td align="center"><a href="#" onClick="if(confirm(\''.$TEXT_DELETE_ATTRIBUTE_FROM_DISC_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=discAttributes&slot='.$slot.'&action=delete&dAtr='.$rowAttributes['PK_Attribute'].'\'">'.$TEXT_REMOVE_CONST.'</a></td>
						</tr>';
			}
			$out.='
				</table></td>
			</tr>			
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'">
				</td>
			</tr>			
			<tr>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>
			
			<tr>
				<td><B>'.$TEXT_ADD_DISC_ATTRIBUTE_CONST.':</B><br>'.pulldownFromArray($attributeTypes,'newAttributeType',@$_POST['newAttributeType'],'onChange="document.discAttributes.action.value=\'form\';document.discAttributes.submit();"').'</td>
				<td><B>'.$TEXT_DISC_ATTRIBUTE_NAME_CONST.'</B><br><input type="text" name="newAttributeName" value=""></td>
			</tr>';
			if(isset($_POST['newAttributeType']) && $_POST['newAttributeType']!='0'){
				$newAttributeType=(int)$_POST['newAttributeType'];
				$resAttributesByType=$mediadbADO->Execute('SELECT * FROM Attribute WHERE FK_AttributeType=? ORDER BY Name ASC',$newAttributeType);
				$out.='
				<tr>
					<td>&nbsp;</td>
					<td><select name="existingAttributes" size="20" onClick="setAttributeName();" onChange="setAttributeName();">';
				while($rowAttributesByType=$resAttributesByType->FetchRow()){
					$out.='<option value="'.$rowAttributesByType['PK_Attribute'].'">'.$rowAttributesByType['Name'].'</option>';
				}
				$out.='
					</select> <input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
				</tr>';
			}
			$out.='
		</table>';
		$out.='
		</form>';
		if(isset($_POST['newAttributeType']) && $_POST['newAttributeType']!='0'){
			$out.='
			<script>
			 	var frmvalidator = new formValidator("discAttributes");			
	 			frmvalidator.addValidation("newAttributeName","req","'.$TEXT_NEW_ATTRIBUTE_VALIDATION_CONST.'");
			</script>		
			';
		}	
	}else{
	// process area

		if(isset($_POST['add'])){
			$newAttributeType=$_POST['newAttributeType'];
			$newAttributeName=cleanString($_POST['newAttributeName']);
			$existingAttributes=(int)@$_POST['existingAttributes'];
			if($existingAttributes!=0){
				$resExistingAttribute=$mediadbADO->Execute('SELECT Name FROM Attribute WHERE PK_Attribute=?',$existingAttributes);
				$rowExistingAttribute=$resExistingAttribute->FetchRow();
				if($rowExistingAttribute['Name']!=$newAttributeName){
					
					// new attribute, insert in Attribute and File_Attribute tables
					$isAdded=addDiscAttribute($newAttributeType,$newAttributeName,$disc,$mediadbADO);
					
					if($isAdded!==true){
						header('Location: index.php?section=discAttributes&fileID='.$fileID.'&error='.$TEXT_ERROR_ATTRIBUTE_NOT_ADDED_CONST.': '.urlencode(nl2br($AttributeAdded)));
						exit();
					}
					
				}else{
					$mediadbADO->Execute('INSERT IGNORE INTO File_Attribute (FK_File, FK_Attribute) VALUES (?,?)',array($fileID,$existingAttributes));
				}
			}else{
				$isAdded=addDiscAttribute($newAttributeType,$newAttributeName,$disc,$mediadbADO);
				if($isAdded!==true){
					header('Location: index.php?section=discAttributes&slot='.$slot.'&error='.$TEXT_ERROR_DISC_ATTRIBUTE_NOT_ADDED_CONST);
					exit();
				}
			}
			
			header('Location: index.php?section=discAttributes&slot='.$slot.'&msg='.$TEXT_DISC_ATTRIBUTE_ADDED_CONST.':<br>'.urlencode(nl2br(@$AttributeAdded)));
			exit();
		}
		
		if(isset($_REQUEST['dAtr'])){
			$deleteAttribute=$_REQUEST['dAtr'];
			$mediadbADO->Execute('DELETE FROM Disc_Attribute WHERE FK_Disc=? AND FK_Attribute=?',array($disc,$deleteAttribute));
			
			header('Location: index.php?section=discAttributes&slot='.$slot.'&msg='.$TEXT_ATTRIBUTE_DELETED_FROM_DISC_CONST);	
			exit();		
		}
	
	
		if(isset($_POST['update'])){
			$type=(int)$_POST['type'];
			$mediadbADO->Execute('UPDATE Disc SET EK_MediaType=? WHERE PK_Disc=?',array($type,$disc));
			header('Location: index.php?section=discAttributes&slot='.$slot.'&msg='.$TEXT_DISC_MEDIA_TYPE_UPDATED_CONST);			
			exit();
		}
		
		
		header('Location: index.php?section=discAttributes&slot='.$slot);			
	}
	$output->setReloadLeftFrame(false);
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function addDiscAttribute($newAttributeType,$newAttributeName,$disc,$mediadbADO){
	/* @var $mediadbADO ADOConnection */
	$res=$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) VALUES (?,?)',array($newAttributeType,$newAttributeName));
	$attrID=$mediadbADO->Insert_ID();
	$res=$mediadbADO->Execute('INSERT INTO Disc_Attribute (FK_Disc,FK_Attribute) VALUES (?,?)',array($disc,$attrID));
	
	if($mediadbADO->Affected_Rows()>0){
		return true;
	}
	
	return false;
}
