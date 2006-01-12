<?
function editPhoneNumber($output,$telecomADO) {
	/* @var $telecomADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$id=(int)@$_REQUEST['id'];

	if ($action == 'form') {
		
		$out.='
		
	<div class="err" align="center">'.stripslashes(@$_GET['error']).'</div>
	<div class="confirm" align="center">'.@$_GET['msg'].'</div>
		
	<form action="index.php" method="POST" name="editPhoneNumber">
	<input type="hidden" name="section" value="editPhoneNumber">
	<input type="hidden" name="action" value="add">	
		'.phoneNumberForm($telecomADO,$userID).'
	</form>
	';
	} else {
		if(isset($_POST['editPhoneNumber'])){
			$contact=((int)@$_POST['contact']>0)?(int)$_POST['contact']:NULL;
			$PhoneType=((int)@$_POST['PhoneType']>0)?(int)$_POST['PhoneType']:NULL;
			$CountryCode=cleanString($_POST['CountryCode']);
			$AreaCode=cleanString($_POST['AreaCode']);
			$PhoneNumber=cleanString($_POST['PhoneNumber']);
			$Extension=cleanString($_POST['Extension']);
			$DialAs=cleanString($_POST['DialAs']);
			
			if($DialAs=='' && $PhoneNumber==''){
				header('Location: index.php?section=editPhoneNumber&error=You must provide either "phone number" or "dial as" fields.');
				exit();
			}
			
			$telecomADO->Execute('
				UPDATE PhoneNumber SET
					`FK_Contact`=?,
					`FK_PhoneType`=?,
					`CountryCode`=?,
					`AreaCode`=?,
					`PhoneNumber`=?,
					`Extension`=?,
					`DialAs`=?
				WHERE PK_PhoneNumber=?',
			array($contact,$PhoneType,$CountryCode,$AreaCode,$PhoneNumber,$Extension,$DialAs,$id));
			$msg='The phone number was changed.';
		}
		
		header("Location: index.php?section=phoneBook&msg=".@$msg);
	}

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array("Phone Book"=>'index.php?section=phoneBook',"Edit phone number"=>'index.php?section=editPhoneNumber&id='.$id));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Phone book');
	$output->output();
}

// multipage display for phone numbers
function phoneNumberForm($telecomADO,$userID){
	/* @var $telecomADO ADOConnection */
	/* @var $res ADORecordSet */
	$contacts=getAssocArray('Contact','PK_Contact','Name',$telecomADO);
	$PhoneTypes=getAssocArray('PhoneType','PK_PhoneType','Description',$telecomADO);
	$id=(int)@$_REQUEST['id'];
	if($id<=0){
		return 'Phone number not found.';
	}
	
	$pnData=getFieldsAsArray('PhoneNumber','FK_Contact,FK_PhoneType,CountryCode,AreaCode,PhoneNumber,Extension,DialAs',$telecomADO,'WHERE PK_PhoneNumber='.$id);

	$out='
	<script>
		function setDialAs(){
			cc=document.editPhoneNumber.CountryCode.value;
			ac=document.editPhoneNumber.AreaCode.value;
			pn=document.editPhoneNumber.PhoneNumber.value;
			dialAs=((cc=="1")?"91":"9011"+cc)+ac+pn;
			document.editPhoneNumber.DialAs.value=dialAs;
		}
	</script>	
	<input type="hidden" name="id" value="'.$id.'">
	<table celspacing="0" cellpadding="3" align="center">
		<tr>
			<td colspan="2"><h3>Edit number</h3></td>
		</tr>
		<tr>
			<td><B>Contact</B></td>
			<td>'.pulldownFromArray($contacts,'contact',$pnData['FK_Contact'][0]).'</td>
		</tr>	
		<tr>
			<td><B>PhoneType</B></td>
			<td>'.pulldownFromArray($PhoneTypes,'PhoneType',$pnData['FK_PhoneType'][0]).'</td>
		</tr>	
		<tr>
			<td><B>Country Code</B></td>
			<td><input type="text" name="CountryCode" value="'.$pnData['CountryCode'][0].'" onkeyup="setDialAs();"></td>
		</tr>	
		<tr>
			<td><B>Area Code</B></td>
			<td><input type="text" name="AreaCode" value="'.$pnData['AreaCode'][0].'" onkeyup="setDialAs();"></td>
		</tr>	
		<tr>
			<td><B>Phone Number</B></td>
			<td><input type="text" name="PhoneNumber" value="'.$pnData['PhoneNumber'][0].'" onkeyup="setDialAs();"></td>
		</tr>	
		<tr>
			<td><B>Extension</B></td>
			<td><input type="text" name="Extension" value="'.$pnData['Extension'][0].'"></td>
		</tr>	
		<tr>
			<td><B>Dial As</B></td>
			<td><input type="text" name="DialAs" value="'.$pnData['DialAs'][0].'"></td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="editPhoneNumber" value="Save changes"></td>
		</tr>	
	</table>
	<script>
	 	var frmvalidator = new formValidator("editPhoneNumber");
		frmvalidator.addValidation("contact","dontselect=0","Please select contact.");
		frmvalidator.addValidation("PhoneType","dontselect=0","Please select phone type.");
	</script>
	';
			
	return $out;
}
?>