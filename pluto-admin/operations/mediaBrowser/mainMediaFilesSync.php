<?
function mainMediaFilesSync($output,$mediadbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$path = (isset($_REQUEST['path']) && $_REQUEST['path']!='')?cleanString($_REQUEST['path']):'';
	
	$typeArray=array();
	$resType=$mediadbADO->Execute('SELECT * FROM Type ORDER BY Description ASC');
	while($rowType=$resType->FetchRow()){
		$typeArray[$rowType['PK_Type']]=$rowType['Description'];
	}
	$notInDBArray=array();	
	
	if($action=='form'){
		if($path!=''){
			$physicalFiles=grabFiles($path);
			
			$out.='<h3>Directory: '.$path.'</h3>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div><br>
			<form action="index.php" method="POST" name="mainMediaFilesSync">
			<input type="hidden" name="section" value="mainMediaFilesSync">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="path" value="'.$path.'">
			
			';
			if(isset($_REQUEST['filename']) && $_REQUEST['filename']!='')
				$out.='<a href="index.php?section=leftMediaFilesSync" target="treeframe"><img src="scripts/treeview/diffDoc.gif" border="0" align="middle">Show directory structure</a>';
			$out.='
				<table cellpading="0" cellspacing="0">
					<tr>
						<td colspan="2"><B>Files on disk</B></td>
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
							<td colspan="2">No files in physical directory</td>
						</tr>';
			else{
				foreach($physicalFiles as $physicalkey => $filename){
					if((in_array($filename,$dbFiles))){
						$key=array_search($filename,$dbFiles);
						$inDB=1;
					}else{
						$notInDBArray[]=$physicalkey; 
						$inDB=0;
						$addToDB='<td align="right"> Type: <select name="type_'.$physicalkey.'">
							<option value="">- Please select -</option>';
						foreach ($typeArray AS $typeID=>$typeDescription){
							$addToDB.='<option value="'.$typeID.'">'.$typeDescription.'</option>';
						}
						$addToDB.='</select> <input type="submit" class="button" name="add" value="Add to database">
						<input type="hidden" name="filename_'.$physicalkey.'" value="'.$filename.'">
						</td>';
					}
					$out.='	<tr style="background-color:'.(($physicalkey%2==0)?'#EEEEEE':'#EBEFF9').';">
								<td '.(((@$inDB==1)?'colspan="2"':'')).'>'.((@$inDB==1)?'<img src=include/images/sync.gif align=middle border=0>':'<img src=include/images/disk.gif align=middle border=0>').' '.((@$inDB==1)?'<a href="index.php?section=editMediaFile&fileID='.$dbPKFiles[$key].'"><B>'.$filename.'</B></a>':'<B>'.$filename.'</B> '.$addToDB).'</td>
							</tr>';
						if(@$inDB==1){	
							$queryAttributes='
								SELECT PK_Attribute, AttributeType.Description AS AttributeName,Name,FirstName
								FROM File_Attribute
									INNER JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute
									INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
								WHERE FK_File=? ORDER BY AttributeType.Description ASC
							';
							$resAttributes=$mediadbADO->Execute($queryAttributes,$dbPKFiles[$key]);
							$attributes='';
							while($rowAttributes=$resAttributes->FetchRow()){
								$attributes.='<b>'.$rowAttributes['AttributeName'].'</b>: <a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties" target="_self">'.stripslashes($rowAttributes['Name']).(($rowAttributes['FirstName']!='')?', '.$rowAttributes['FirstName']:'').'</a> ';
							}
							$out.='
							<tr style="background-color:'.(($physicalkey%2==0)?'#EEEEEE':'#EBEFF9').';">
								<td colspan="2">'.@$attributes.'</td>
							</tr>';
							
						}
				}
			}
			$out.='
				<tr>
					<td colspan="2"><B>Files in database only</B></td>
				</tr>';
			$dbonlyFiles=array_diff($dbFiles,$physicalFiles);
			if(count($dbonlyFiles)==0)
				$out.='	<tr>
							<td colspan="2">No other files in database</td>
						</tr>';
			foreach($dbonlyFiles as $dbkey => $filename){
				$queryAttributes='
					SELECT PK_Attribute, AttributeType.Description AS AttributeName,Name,FirstName
					FROM File_Attribute
						INNER JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute
						INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType
					WHERE FK_File=? ORDER BY AttributeType.Description ASC
					';
				$resAttributes=$mediadbADO->Execute($queryAttributes,$dbPKFiles[$dbkey]);
				$attributes='';
				while($rowAttributes=$resAttributes->FetchRow()){
					$attributes.='<b>'.$rowAttributes['AttributeName'].'</b>: <a href="index.php?section=mainMediaBrowser&attributeID='.$rowAttributes['PK_Attribute'].'&action=properties" target="_self">'.stripslashes($rowAttributes['Name']).(($rowAttributes['FirstName']!='')?', '.$rowAttributes['FirstName']:'').'</a> ';
				}
	
				$out.='	<tr style="background-color:'.(($dbkey%2==0)?'#EEEEEE':'#EBEFF9').';">
							<td colspan="2"><img src=include/images/db.gif align=middle border=0> <a href="index.php?section=editMediaFile&fileID='.@$dbPKFiles[$key].'"><B>'.$filename.'</B></a></td>
						</tr>
						<tr style="background-color:'.(($dbkey%2==0)?'#EEEEEE':'#EBEFF9').';">
							<td colspan="2">'.@$attributes.'</td>
						</tr>';
			}
			$out.='
				<tr>
					<td colspan="2">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="2"><B>LEGEND FOR FILES</B><br>
					<img src=include/images/disk.gif align=middle border=0>	Exist only on disk<br>
					<img src=include/images/db.gif align=middle border=0> Exist only in database<br>
					<img src=include/images/sync.gif align=middle border=0>	Exist both on disk and in database
				</tr>
			</table>';
		}
	$out.='
			<input type="hidden" name="notInDBArray" value="'.join(',',$notInDBArray).'">
		</form>
	';
		
	}else{
	// process area
		$notInDBArray=explode(',',$_POST['notInDBArray']);
		foreach($notInDBArray AS $physicalkey){
			$filename=$_POST['filename_'.$physicalkey];
			$type=(int)$_POST['type_'.$physicalkey];
			if($type!=0)
				$mediadbADO->Execute('INSERT INTO File (FK_Type, Path, Filename) VALUES (?,?,?)',array($type,stripslashes($path),$filename));
		}
		header('Location: index.php?section=mainMediaFilesSync&path='.$path.'&msg=File added to database.');
	}
		
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

