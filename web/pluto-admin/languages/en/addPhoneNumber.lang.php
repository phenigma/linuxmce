<?
function addPhoneNumber($output,$telecomADO) {
	/* @var $telecomADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;

	if ($action == 'form') {
		
		$out.='
		
	<div class="err" align="center">'.stripslashes(@$_GET['error']).'</div>
	<div class="confirm" align="center">'.@$_GET['msg'].'</div>
		
	<form action="index.php" method="POST" name="addPhoneNumber">
	<input type="hidden" name="section" value="addPhoneNumber">
	<input type="hidden" name="action" value="add">	
		'.phoneNumberForm($telecomADO,$userID).'
	</form>
	';
	} else {
		if(isset($_POST['addPhoneNumber'])){
			$contact=((int)@$_POST['contact']>0)?(int)$_POST['contact']:NULL;
			$PhoneType=((int)@$_POST['PhoneType']>0)?(int)$_POST['PhoneType']:NULL;
			$CountryCode=cleanString($_POST['CountryCode']);
			$AreaCode=cleanString($_POST['AreaCode']);
			$PhoneNumber=cleanString($_POST['PhoneNumber']);
			$Extension=cleanString($_POST['Extension']);
			$DialAs=cleanString($_POST['DialAs']);
			if($DialAs=='' && $PhoneNumber==''){
				header('Location: index.php?section=addPhoneNumber&error=You must provide either "phone number" or "dial as" fields.');
				exit();
			}
			
			$telecomADO->Execute('
				INSERT INTO PhoneNumber
					(`FK_Contact`, `FK_PhoneType`, `CountryCode`, `AreaCode`, `PhoneNumber`, `Extension`, `DialAs`)
				VALUES
					(?,?,?,?,?,?,?)',
			array($contact,$PhoneType,$CountryCode,$AreaCode,$PhoneNumber,$Extension,$DialAs));
			$msg='The phone number was added.';
			header("Location: index.php?section=phoneBook&msg=".@$msg);
			exit();
		}
		
		if(isset($_POST['addContact'])){
			$Name=cleanString($_POST['Name']);
			$Company=cleanString($_POST['Company']);
			$Title=cleanString($_POST['Title']);
			$EK_Users=(isset($_POST['EK_Users']) && (int)$_POST['EK_Users']>0)?(int)$_POST['EK_Users']:NULL;
			
			$telecomADO->Execute('
				INSERT INTO Contact
					(`Name`, `Company`, `Title`, `EK_Users`)
				VALUES
					(?,?,?,?)',
			array($Name,$Company,$Title,$EK_Users));
			$msg='The contact was added.';
		}	
		header("Location: index.php?section=addPhoneNumber&msg=".@$msg);
	}

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array("Phone Book"=>'index.php?section=phoneBook',"Add phone number"=>'index.php?section=addPhoneNumber'));
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
	
	$out='
	<table celspacing="0" cellpadding="3" align="center">
		<tr>
			<td colspan="2"><h3>Add phone number</h3></td>
		</tr>
		<tr>
			<td><B>Contact</B></td>
			<td>'.pulldownFromArray($contacts,'contact',0).'</td>
		</tr>	
		<tr>
			<td><B>PhoneType</B></td>
			<td>'.pulldownFromArray($PhoneTypes,'PhoneType',0).'</td>
		</tr>	
		<tr>
			<td><B>Country Code</B></td>
			<td><input type="text" name="CountryCode" value=""></td>
		</tr>	
		<tr>
			<td><B>Area Code</B></td>
			<td><input type="text" name="AreaCode" value=""></td>
		</tr>	
		<tr>
			<td><B>Phone Number</B></td>
			<td><input type="text" name="PhoneNumber" value=""></td>
		</tr>	
		<tr>
			<td><B>Extension</B></td>
			<td><input type="text" name="Extension" value=""></td>
		</tr>	
		<tr>
			<td><B>Dial As</B></td>
			<td><input type="text" name="DialAs" value=""></td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="addPhoneNumber" value="Add phone number"></td>
		</tr>	
	<script>
		 	var frmvalidator = new formValidator("addPhoneNumber");
			frmvalidator.addValidation("contact","dontselect=0","Please select contact.");
 			frmvalidator.addValidation("PhoneType","dontselect=0","Please select phone type.");
	</script>
	
	</form>
	<form action="index.php" method="POST" name="addContactForm">
	<input type="hidden" name="section" value="addPhoneNumber">
	<input type="hidden" name="action" value="add">	
		<tr>
			<td colspan="2"><hr></td>
		</tr>	
		<tr>
			<td colspan="2"><h3>Add contact</h3></td>
		</tr>
		<tr>
			<td><B>Name</B></td>
			<td><input type="text" name="Name" value=""></td>
		</tr>	
		<tr>
			<td><B>Company</B></td>
			<td><input type="text" name="Company" value=""></td>
		</tr>	
		<tr>
			<td><B>Title</B></td>
			<td><input type="text" name="Title" value=""></td>
		</tr>	
		<tr>
			<td><B>Public contact</B></td>
			<td><input type="checkbox" name="EK_Users" value="'.$userID.'"></td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="addContact" value="Add contact"></td>
		</tr>
	</table>
		<script>
		 	var frmvalidator = new formValidator("addContactForm");
 			frmvalidator.addValidation("Name","req","Please type contact name.");
		</script>			
	';
			
	return $out;
}
?>