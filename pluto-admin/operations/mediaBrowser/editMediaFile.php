<?
function editMediaFile($output,$mediadbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$fileID=$_REQUEST['fileID'];
	
	$queryAttrTypes='SELECT * FROM AttributeType ORDER BY Description ASC';
	$resAttrTypes=$mediadbADO->Execute($queryAttrTypes);
	$attributeTypes=array();
	while($rowTypes=$resAttrTypes->FetchRow()){
		$attributeTypes[$rowTypes['PK_AttributeType']]=$rowTypes['Description'];
	}
	
	$scriptInHead='
	<script>
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
	</script>
	';
	
	if($action=='form'){
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
			<form action="index.php" method="POST" name="editMediaFile">
				<input type="hidden" name="section" value="editMediaFile">
				<input type="hidden" name="action" value="update">
				<input type="hidden" name="fileID" value="'.$fileID.'">
		';
		$resFile=$mediadbADO->Execute('SELECT * FROM File WHERE PK_File=?',$fileID);
		$rowFile=$resFile->FetchRow();
		$out.='
		<h4>Edit Media file</h4>
		<table border="0" cellspacing="0" cellpadding="3">
			<tr bgColor="#EEEEEE">
				<td><B>File:</B></td>
				<td>'.$rowFile['Filename'].'</td>
			</tr>
			<tr bgcolor="#EBEFF9">
				<td><B>Location:</B></td>
				<td>'.((file_exists($rowFile['Path'])?'<img src=include/images/sync.gif align=middle border=0>':'<img src=include/images/db.gif align=middle border=0>')).$rowFile['Path'].'</td>
			</tr>
			<tr bgcolor="#EEEEEE">
				<td><B>Type:</B></td>
				<td><select name="type">';
			$resType=$mediadbADO->Execute('SELECT * FROM Type ORDER BY Description ASC');
			while($rowType=$resType->FetchRow()){
				$out.='<option value="'.$rowType['PK_Type'].'" '.(($rowType['PK_Type']==$rowFile['FK_Type'])?'selected':'').'>'.$rowType['Description'].'</option>';
			}
			$out.='
				</select></td>
			</tr>
			<tr bgcolor="#EBEFF9">
				<td valign="top"><B>Attributes:</B></td>
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
							<td>'.$rowAttributes['Name'].(($rowAttributes['FirstName']!='')?', '.$rowAttributes['FirstName']:'').'</td>
							<td align="center"><a href="#" onClick="if(confirm(\'Are you sure you want to delete this attribute from the file?\'))self.location=\'index.php?section=editMediaFile&fileID='.$fileID.'&action=delete&dAtr='.$rowAttributes['PK_Attribute'].'\'">Remove</a></td>
						</tr>';
			}
			$out.='
				</table></td>
			</tr>			
			<tr>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td><B>Add attribute:</B><br><select name="newAttributeType" onChange="document.editMediaFile.action.value=\'form\';document.editMediaFile.submit();">
					<option value="0">- Please select -</option>';
			foreach($attributeTypes AS $attributeID=>$attributeName){
				$out.='<option value="'.$attributeID.'" '.(($attributeID==@$_POST['newAttributeType'])?'selected':'').'>'.$attributeName.'</option>';
			}
			$out.='</select></td>
				<td><B>Attribute name</B><br><input type="text" name="newAttributeName" value=""></td>
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
					</select> <input type="submit" name="add" value="Add"></td>
				</tr>';
			}
			$out.='
		</table>';
		$out.='
		</form>
		<script>
		 	var frmvalidator = new formValidator("editMediaFile");			
 			frmvalidator.addValidation("newAttributeName","req","Please enter a name for attribute or select it from the list below!");
		</script>		
		';
	
	}else{
	// process area
	
		if(isset($_POST['add'])){
			$newAttributeType=$_POST['newAttributeType'];
			$newAttributeName=cleanString($_POST['newAttributeName']);
			$existingAttributes=(int)$_POST['existingAttributes'];
			if($existingAttributes!=0){
				$resExistingAttribute=$mediadbADO->Execute('SELECT Name FROM Attribute WHERE PK_Attribute=?',$existingAttributes);
				$rowExistingAttribute=$resExistingAttribute->FetchRow();
				if($rowExistingAttribute['Name']!=$newAttributeName){
					// new attribute, insert in Attribute and File_Attribute tables
					$mediadbADO->Execute('INSERT INTO Atttribute (FK_AttributeType,Name) VALUES (?,?)',array($newAttributeType,$newAttributeName));
					$insertID=$mediadbADO->Insert_ID();
					
					$mediadbADO->Execute('INSERT INTO File_Attribute (FK_File, FK_Attribute) VALUES (?,?)',array($fileID,$insertID));
				}else{
					$mediadbADO->Execute('INSERT INTO File_Attribute (FK_File, FK_Attribute) VALUES (?,?)',array($fileID,$existingAttributes));
				}
				
			}else{
				$mediadbADO->Execute('INSERT INTO Atttribute (FK_AttributeType,Name) VALUES (?,?)',array($newAttributeType,$newAttributeName));
				$insertID=$mediadbADO->Insert_ID();
				
				$mediadbADO->Execute('INSERT INTO File_Attribute (FK_File, FK_Attribute) VALUES (?,?)',array($fileID,$insertID));
			}
			
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg=File attribute added.');			
		}
		
		if(isset($_REQUEST['dAtr'])){
			$deleteAttribute=$_REQUEST['dAtr'];
			$mediadbADO->Execute('DELETE FROM File_Attribute WHERE FK_File=? AND FK_Attribute=?',array($fileID,$deleteAttribute));
			
			header('Location: index.php?section=editMediaFile&fileID='.$fileID.'&msg=Attribute deleted from this file.');			
		}
	
	}
	
	$output->setScriptInHead($scriptInHead);	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}