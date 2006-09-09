<?
function matchCoverArt($output,$mediadbADO) {
	global $mediaTypes;
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/coverArt.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	
	
	if($action=='form'){
		$scannedArray=array();
		$rs=$mediadbADO->Execute('
			SELECT FK_CoverArtScan,PK_CoverArtScanEntry,FK_File,FK_Disc,FK_Attribute,File.Path,File.Filename,Disc.Slot,Attribute.Name,Engine 
			FROM CoverArtScanEntry 
			INNER JOIN CoverArtScan ON FK_CoverArtScan=PK_CoverArtScan
			LEFT JOIN File ON FK_File=PK_File
			LEFT JOIN Disc ON FK_Disc=PK_Disc
			LEFT JOIN Attribute ON FK_Attribute=PK_Attribute');
		while($row=$rs->FetchRow()){
			$scannedArray[$row['FK_CoverArtScan']]['FK_File']=$row['FK_File'];
			$scannedArray[$row['FK_CoverArtScan']]['FK_Disc']=$row['FK_Disc'];
			$scannedArray[$row['FK_CoverArtScan']]['FK_Attribute']=$row['FK_Attribute'];
			$scannedArray[$row['FK_CoverArtScan']]['Path']=$row['Path'];
			$scannedArray[$row['FK_CoverArtScan']]['Filename']=$row['Filename'];
			$scannedArray[$row['FK_CoverArtScan']]['Slot']=$row['Slot'];
			$scannedArray[$row['FK_CoverArtScan']]['Name']=$row['Name'];
			$scannedArray[$row['FK_CoverArtScan']]['Engine']=$row['Engine'];
			$scannedArray[$row['FK_CoverArtScan']]['Entries'][]=$row['PK_CoverArtScanEntry'];
		}
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<form action="index.php" method="POST" name="matchCoverArt">
			<input type="hidden" name="section" value="matchCoverArt">
			<input type="hidden" name="action" value="update">
		<a href="index.php?section=coverArt">'.$TEXT_COVERART_CONST.'</a><br><br>
		'.formatScannedItems($scannedArray).'
		
		</form>';
	}else{
		$ids=explode(',',$_POST['ids']);
		foreach ($ids AS $id){
			$selPic=$_POST['pic_'.$id];
			if($selPic==0){
				deleteScans($id,$mediadbADO);
			}else{
				$itemType=$_POST['itemType_'.$id];
				$itemValue=$_POST['itemValue_'.$id];

				$urls=getAssocArray('CoverArtScanEntry','PK_CoverArtScanEntry','URL',$mediadbADO,'WHERE FK_CoverArtScan='.$id);
				
				$mediadbADO->Execute('INSERT INTO Picture (Extension,URL) VALUES (?,?)',array('jpg',$urls[$selPic]));
				$pictureID=$mediadbADO->Insert_ID();
				
				$cmd='sudo -u root cp /home/coverartscan/'.$selPic.'.jpg '.$GLOBALS['mediaPicsPath'].$pictureID.'.jpg';
				exec_batch_command($cmd);

				$cmd='sudo -u root cp /home/coverartscan/'.$selPic.'_tn.jpg '.$GLOBALS['mediaPicsPath'].$pictureID.'_tn.jpg';
				exec_batch_command($cmd);
				
				$mediadbADO->Execute('INSERT INTO Picture_'.$itemType.' (FK_'.$itemType.',FK_Picture) VALUES (?,?)',array($itemValue,$pictureID));
				if($itemType=='File' || $itemType=='Disc'){
					syncAttributes($itemType,$itemValue,$selPic,$mediadbADO);
				}
				deleteScans($id,$mediadbADO);
				
			}
		}
		
		header('Location: index.php?section=matchCoverArt&msg='.urlencode($TEXT_COVER_ARTS_UPDATED_CONST));
		exit();
	}

	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_MATCHCOVERART_CONST);
	$output->setNavigationMenu(array($TEXT_MATCHCOVERART_CONST=>'index.php?section=matchCoverArt'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MATCHCOVERART_CONST);
	$output->output();
}

function formatScannedItems($scannedArray){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/coverArt.lang.php');
		
	$out='<table>';
	if(count($scannedArray)==0){
		return 'No scanned items.';
	}
	
	foreach ($scannedArray AS $id=>$data){
		if(count($data['Entries'])==0){
			$picsTable='No covert arts found';
		}else{
			$picsTable='<table>
				<tr>
					<td valign="top" align="center"><input type="radio" name="pic_'.$id.'" value="0" checked><br>None of this</td>';
			$pos=0;
			foreach ($data['Entries'] AS $pic){
				$pos++;
				if(file_exists('/home/coverartscan/'.$pic.'_tn.jpg')){
					$picsTable.='<td align="center"><input type="radio" name="pic_'.$id.'" value="'.$pic.'"><br><a href="coverartscan/'.$pic.'.jpg" target="_blank"><img src="coverartscan/'.$pic.'_tn.jpg" border="0"></a></td>';
				}
				if($pos==6){
					$picsTable.='</tr><tr>';
					$pos=0;
				}
			}
			$picsTable.='</tr></table>';
		}
		
		if($data['FK_File']!=''){
			$itemType='File';
			$itemValue=$data['FK_File'];
		}
		if($data['FK_Disc']!=''){
			$itemType='Disc';
			$itemValue=$data['FK_Disc'];
		}
		if($data['FK_Attribute']!=''){
			$itemType='Attribute';
			$itemValue=$data['FK_Attribute'];
		}
		
		$itemLabel=str_replace(array('Amazon | '),'',$data['Engine']);
		
		
		$out.='
		<input type="hidden" name="itemType_'.$id.'" value="'.$itemType.'">
		<input type="hidden" name="itemValue_'.$id.'" value="'.$itemValue.'">
		<tr>
			<td class="alternate_back">'.$itemLabel.': '.$data['Path'].$data['Filename'].$data['Slot'].$data['Name'].'</td>
		</tr>
		<tr>
			<td>'.$picsTable.'</td>
		</tr>
		
		';
	}
	$out.='
		<tr>
			<td align="center" class="alternate_back"><input type="submit" class="button" name="match" value="'.$TEXT_MATCHCOVERART_CONST.'"></td>
		</tr>	
	</table>
	<input type="hidden" name="ids" value="'.join(',',array_keys($scannedArray)).'">';
	
	return $out;
}

function deleteScans($id,$mediadbADO){
	$picsToDel=array_keys(getAssocArray('CoverArtScanEntry','PK_CoverArtScanEntry','PK_CoverArtScanEntry',$mediadbADO,'WHERE FK_CoverArtScan='.$id));
	foreach ($picsToDel AS $pic){
		@unlink('/home/coverartscan/'.$pic.'.jpg');
		@unlink('/home/coverartscan/'.$pic.'_tn.jpg');
	}
	$mediadbADO->Execute('DELETE FROM CoverArtScanEntry WHERE FK_CoverArtScan=?',array($id));
	$mediadbADO->Execute('DELETE FROM CoverArtScan WHERE PK_CoverArtScan=?',array($id));
}

function syncAttributes($table,$itemValue,$id,$mediadbADO){
	$attributeTypes=getAssocArray('AttributeType','Description','PK_AttributeType',$mediadbADO);
	
	$existingAttributes=getAssocArray('Attribute','AttributeType.Description AS Description','Attribute.Name',$mediadbADO,'INNER JOIN '.$table.'_Attribute ON FK_Attribute=PK_Attribute INNER JOIN AttributeType ON FK_AttributeType=PK_AttributeType WHERE FK_'.$table.'='.$itemValue);
	$amazonAttributes=getAssocArray('CoverArtScanEntry','PK_CoverArtScanEntry','Attributes',$mediadbADO,'WHERE PK_CoverArtScanEntry='.$id);
	$amazonAttributesArray=explode("\n",$amazonAttributes[$id]);
	
	foreach ($amazonAttributesArray AS $line){
		$parts=explode("\t",trim($line));
		// todo: attribute mapping
		if(count($parts)==2){
			$atype=str_replace(array('Average Customer Review', 'Actor','Directors','Artist'),array('Rating', 'Performer','Director','Performer'),$parts[0]);
			$aname=$parts[1];

			if(!in_array($atype,array_keys($existingAttributes)) && isset($attributeTypes[$atype])){

				$mediadbADO->Execute('INSERT INTO Attribute (FK_AttributeType,Name) VALUES (?,?)',array($attributeTypes[$atype],$aname));
				$aid=$mediadbADO->Insert_Id();
				$mediadbADO->Execute('INSERT INTO '.$table.'_Attribute (FK_'.$table.',FK_Attribute) VALUES (?,?)',array($itemValue,$aid));
			}
		}
	}
}
?>