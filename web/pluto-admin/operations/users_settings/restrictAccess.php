<?
function restrictAccess($output,$dbADO) {

	/* @var $dbADO ADOConnection */
	// WARNING: the checkboxes behavior is reversed, checked = Room_User doesn't exist
	$out='';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login");
	}
	$installationID = cleanInteger($_SESSION['installationID']);
	$roomsArray=getArrayFromTable('Room','PK_Room','Description',$dbADO,'WHERE HideFromOrbiter=0 AND FK_Installation='.$installationID,'ORDER BY Description ASC');
	$usersArray=getArrayFromTable('Users','PK_Users','Username',$dbADO,'INNER JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation='.$installationID,'ORDER BY Username ASC');
	$orbitersArray=getDevicesArrayFromCategory($GLOBALS['rootOrbiterID'],$dbADO);
	$addToRoom=(int)@$_REQUEST['addToRoom'];
	
	$out.='<h3>Restrict access</h3>
	<p>Normally anybody can control any room by switching to it, and anybody can switch to being another on the orbiter and access that users private media.  You can restrict these by 
	<ul>
		<li>1) limiting who can control a room and/or which orbiters can be used to control it, and </li>
		<li>2) requiring a pin number to switch to a particular user</li>
	</ul></p>';

	if ($action=='form') {
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,"",attributes);
			}
		</script>
		<p class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'
		<div align="left" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
		<form action="index.php" method="post" name="restrictAccess">
		<input type="hidden" name="section" value="restrictAccess">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="addToRoom" value="'.@$_REQUEST['addToRoom'].'">
		<input type="hidden" name="delR" value="">
		
		<p><B>Rooms</B>
		<table cellpadding="3" cellspacing="0">
			<tr>
				<td colspan="2"><B>-- Restrict access to rooms --</B></td>
			</tr>';
		foreach($roomsArray AS $roomID=>$roomName){
			$rs=$dbADO->Execute('SELECT * FROM Room_Users WHERE FK_Room=?',$roomID);
			$restrictions='';
			if($rs->RecordCount()==0){
				$roomLabel='Full access to everyone.';
				$roomButton='<input type="button" class="button" name="addRestrict" value="Restrict access to" onClick="document.restrictAccess.action.value=\'form\';document.restrictAccess.addToRoom.value='.$roomID.';document.restrictAccess.submit();">';
			}else{
				$roomLabel='Only allow access to';
				$roomButton='<input type="button" class="button" name="addRestrict" value="Add access" onClick="document.restrictAccess.action.value=\'form\';document.restrictAccess.addToRoom.value='.$roomID.';document.restrictAccess.submit();">';
				while($rowR=$rs->FetchRow()){
					$restrictions.='
				<tr>
					<td>User: '.pulldownFromArray($usersArray,'user',$rowR['FK_Users'],'','key','-- Any --').'</td>
					<td>using orbiter '.pulldownFromArray($orbitersArray,'orbiter',$rowR['FK_Orbiter'],'','key','-- Any --').' <input type="button" class="button" name="del" value="Delete" onClick="document.restrictAccess.delR.value='.$rowR['PK_Room_Users'].';document.restrictAccess.submit();"></td>
				</tr>';
				}
			}
			
			$out.='
			<tr>
				<td bgcolor="lightblue" colspan="2"><B>'.$roomName.'</B></td>
			</tr>
			<tr>
				<td>'.$roomLabel.'</td>
				<td align="right">'.$roomButton.'</td>
			</tr>';
			$out.=@$restrictions;
			
			if($addToRoom==$roomID){
				$out.='
				<tr bgcolor="lightyellow">
					<td>User: '.pulldownFromArray($usersArray,'user',0,'','key','-- Any --').'</td>
					<td>using orbiter '.pulldownFromArray($orbitersArray,'orbiter',0,'','key','-- Any --').' <input type="submit" class="button" name="submit" value="Save"></td>
				</tr>';
			}
			$out.='
			<tr>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
			</tr>';
		}
		$out.='
		</table>
		</form>';
	} else {
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=restrictAccess&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		$user=((int)$_POST['user']>0)?(int)$_POST['user']:NULL;
		$orbiter=((int)$_POST['orbiter']>0)?(int)$_POST['orbiter']:NULL;
		if($addToRoom!=0){
			$dbADO->Execute('INSERT INTO Room_Users (FK_Room,FK_Users,FK_Orbiter) VALUES (?,?,?)',array($addToRoom,$user,$orbiter));
		}

		$delR=(int)@$_POST['delR'];
		if($delR!=0){
			$dbADO->Execute('DELETE FROM Room_Users WHERE PK_Room_Users=?',$delR);
		}
		
		header("Location: index.php?section=restrictAccess&msg=Restrictions updated.");

	}

	

	$output->setNavigationMenu(array("Restrict Access"=>'index.php?section=restrictAccess'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Restrict access');			
	$output->output();  		
}
?>