<?
function mainMediaFilesSync($output,$mediadbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$path = (isset($_REQUEST['path']) && $_REQUEST['path']!='')?cleanString($_REQUEST['path']):'';
	if($path!=''){
		$physicalFiles=grabFiles($path);
		
		$out.='<h3>Directory: '.$path.'</h3>';
		if(isset($_REQUEST['filename']) && $_REQUEST['filename']!='')
			$out.='<a href="index.php?section=leftMediaFilesSync" target="treeframe"><img src="scripts/treeview/diffDoc.gif" border="0" align="middle">Show directory structure</a>';
		$out.='
			<table>
				<tr>
					<td><B>Files on disk</B></td>
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
						<td>No files in physical directory</td>
					</tr>';
		else{
			foreach($physicalFiles as $physicalkey => $filename){
				$out.='	<tr style="background-color:'.(($physicalkey%2==0)?'#EEEEEE':'#EBEFF9').';">
							<td>'.((in_array($filename,$dbFiles))?'<img src=include/images/sync.gif align=middle border=0>':'<img src=include/images/disk.gif align=middle border=0>').' <a href="index.php?section=editMediaFile&fileID='.$dbPKFiles[$key].'"><B>'.$filename.'</B></a></td>
						</tr>';
					if((in_array($filename,$dbFiles))){
						$key=array_search($filename,$dbFiles);
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
							<td>'.@$attributes.'</td>
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
						<td><img src=include/images/db.gif align=middle border=0> <B>'.$filename.'</B></td>
					</tr>
					<tr style="background-color:'.(($dbkey%2==0)?'#EEEEEE':'#EBEFF9').';">
						<td>'.@$attributes.'</td>
					</tr>';
		}
		$out.='
			<tr>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td><B>LEGEND FOR FILES</B><br>
				<img src=include/images/disk.gif align=middle border=0>	Exist only on disk<br>
				<img src=include/images/db.gif align=middle border=0> Exist only in database<br>
				<img src=include/images/sync.gif align=middle border=0>	Exist both on disk and in database
			</tr>';
	}	
	$out.='
		</table>
	';
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

