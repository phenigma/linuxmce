<?
function phoneBook($output,$telecomADO) {
	/* @var $telecomADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;

	if ($action == 'form') {
		
		$out.='
		
	<div class="err">'.@$_GET['error'].'</div>
	<div class="confirm">'.@$_GET['msg'].'</div>
		
	<form action="index.php" method="POST" name="phoneBook">
	<input type="hidden" name="section" value="phoneBook">
	<input type="hidden" name="action" value="add">	
	<h3>Phone Book</h3>
		'.phonesTable($telecomADO,$userID,$page).'
	</form>
	';
	} else {

		// process delete
		$did=(int)@$_REQUEST['did'];
		if($did>0){
			$telecomADO->Execute('DELETE FROM PhoneNumber WHERE PK_PhoneNumber=?',$did);
			$msg='The phone number was deleted.';
		}
			
		header("Location: index.php?section=phoneBook&msg=".@$msg);
	}

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array("Phone Book"=>'index.php?section=phoneBook'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Phone book');
	$output->output();
}

// multipage display for phone numbers
function phonesTable($telecomADO,$userID,$page){
	/* @var $telecomADO ADOConnection */
	/* @var $res ADORecordSet */
	$records_per_page=10;
			
	$res=$telecomADO->Execute('
		SELECT * 
		FROM PhoneNumber
		INNER JOIN PhoneType ON FK_PhoneType = PK_PhoneType
		INNER JOIN Contact ON FK_Contact=PK_Contact
		WHERE EK_Users IS NULL OR EK_Users=?
		ORDER BY Name ASC,PhoneNumber ASC
		',$userID);
	$totalRecords=$res->RecordCount();
	if($totalRecords==0){
		return 'No phone numbers available.<br> <input type="button" class="button" name="add" value="Add phone number/contact" onClick="self.location=\'index.php?section=addPhoneNumber\'">';
	}
	
	$noPages=ceil($totalRecords/$records_per_page);
	$start=($page-1)*$records_per_page+1;
	$end=$page*$records_per_page;
	$end=($end>$totalRecords)?$totalRecords:$end;

	$pageLinks='Pages: ';
	for($i=1;$i<=$noPages;$i++){
		$pageLinks.=($i==$page)?' '.$i:' <a href="index.php?section=phoneBook&page='.$i.'">'.$i.'</a>';
	}
	
	$out='
		<table celspacing="0" cellpadding="3" align="center">
			<tr>
				<td colspan="2">Records '.$start.'-'.$end.' of '.$totalRecords.'</td>
				<td colspan="9" align="right">'.$pageLinks.'</td>
			</tr>
			<tr bgcolor="lightblue">
				<td align="center"><B>Contact name</B></td>
				<td align="center"><B>Company</B></td>
				<td align="center"><B>Title</B></td>
				<td align="center"><B>Phone type</B></td>	
				<td align="center"><B>CC*</B></td>	
				<td align="center"><B>AC**</B></td>
				<td align="center"><B>Number</B></td>
				<td align="center"><B>Extension</B></td>
				<td align="center"><B>Dial As</B></td>	
				<td align="center"><B>Public</B></td>	
				<td align="center"><B>Function</B></td>	
			</tr>';
	$pos=0;
	while($row=$res->FetchRow()){
		$pos++;
		
		if($pos>=$start && $pos<=$end){
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';		
			
			$out.='
			<tr bgcolor="'.$color.'">
				<td align="center"><a href="index.php?section=editContact&cid='.$row['FK_Contact'].'">'.$row['Name'].'</a></td>
				<td align="center">'.$row['Company'].'</td>
				<td align="center">'.$row['Title'].'</td>
				<td align="center">'.$row['Description'].'</td>	
				<td align="center">'.$row['CountryCode'].'</td>	
				<td align="center">'.$row['AreaCode'].'</td>
				<td align="center">'.$row['PhoneNumber'].'</td>
				<td align="center">'.$row['Extension'].'</td>
				<td align="center">'.$row['DialAs'].'</td>	
				<td align="center">'.((is_null($row['EK_Users']))?'Y':'N').'</td>	
				<td align="center">
					<input type="button" class="button" name="edit" value="Edit" onClick="self.location=\'index.php?section=editPhoneNumber&id='.$row['PK_PhoneNumber'].'\'"> 
					<input type="button" class="button" name="delete" value="Delete" onclick="if(confirm(\'Are you sure you want to delete this number?\'))self.location=\'index.php?section=phoneBook&action=del&did='.$row['PK_PhoneNumber'].'\'"></td>	
			</tr>';
		}

	}
	$out.='
		<tr>
			<td colspan="2">Records '.$start.'-'.$end.' of '.$totalRecords.'</td>
			<td colspan="9" align="right">'.$pageLinks.'</td>
		</tr>	
		<tr>
			<td colspan="11" align="center"><input type="button" class="button" name="add" value="Add phone number/contact" onClick="self.location=\'index.php?section=addPhoneNumber\'"></td>
		</tr>	
		<tr>
			<td colspan="11" align="left">	* CC=Country code<br>
	** AC=Area code</em></td>
		</tr>	
	</table>
	<em>';
			
	return $out;
}
?>