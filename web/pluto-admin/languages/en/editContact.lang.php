<?
function editContact($output,$telecomADO) {
	/* @var $telecomADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$cid=(int)@$_REQUEST['cid'];

	if ($action == 'form') {
		
		$out.='
		
	<div class="err" align="center">'.@$_GET['error'].'</div>
	<div class="confirm" align="center">'.@$_GET['msg'].'</div>
		
	<form action="index.php" method="POST" name="editContact">
	<input type="hidden" name="section" value="editContact">
	<input type="hidden" name="action" value="add">	
		'.phoneNumberForm($telecomADO,$userID,$cid).'
	</form>
	';
	} else {
		
		if(isset($_POST['editContact'])){
			$Name=cleanString($_POST['Name']);
			$Company=cleanString($_POST['Company']);
			$Title=cleanString($_POST['Title']);
			$EK_Users=(isset($_POST['EK_Users']) && (int)$_POST['EK_Users']>0)?(int)$_POST['EK_Users']:NULL;
			
			$telecomADO->Execute('
				UPDATE Contact SET
					`Name`=?,
					`Company`=?,
					`Title`=?,
					`EK_Users`=?
				WHERE PK_Contact=?',
			array($Name,$Company,$Title,$EK_Users,$cid));
			$msg='The contact was updated.';
		}	
		header("Location: index.php?section=phoneBook&msg=".@$msg);
	}

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array("Phone Book"=>'index.php?section=phoneBook',"Edit contact"=>'index.php?section=editContact&cid='.$cid));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Phone book');
	$output->output();
}

// multipage display for phone numbers
function phoneNumberForm($telecomADO,$userID,$cid){
	/* @var $telecomADO ADOConnection */
	/* @var $res ADORecordSet */
	if($cid<=0){
		return 'Contact not found.';
	}	

	$cData=getFieldsAsArray('Contact','Name,Company,Title,EK_Users',$telecomADO,'WHERE PK_Contact='.$cid);
	
	$out='
	<input type="hidden" name="cid" value="'.$cid.'">	
	<table celspacing="0" cellpadding="3" align="center">
		<tr>
			<td colspan="2"><h3>Edit contact</h3></td>
		</tr>
		<tr>
			<td><B>Name</B></td>
			<td><input type="text" name="Name" value="'.$cData['Name'][0].'"></td>
		</tr>	
		<tr>
			<td><B>Company</B></td>
			<td><input type="text" name="Company" value="'.$cData['Company'][0].'"></td>
		</tr>	
		<tr>
			<td><B>Title</B></td>
			<td><input type="text" name="Title" value="'.$cData['Title'][0].'"></td>
		</tr>	
		<tr>
			<td><B>Public contact</B></td>
			<td><input type="checkbox" name="EK_Users" value="'.$userID.'" '.((is_null($cData['EK_Users'][0]))?'checked':'').'></td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="editContact" value="Save changes"></td>
		</tr>
	</table>
		<script>
		 	var frmvalidator = new formValidator("editContact");
 			frmvalidator.addValidation("Name","req","Please type contact name.");
		</script>			
	';
			
	return $out;
}
?>