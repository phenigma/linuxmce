<?
function alertTypes($output,$securitydbADO) {
	/* @var $securitydbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login");
	}
		$installationID = cleanInteger($_SESSION['installationID']);
	
	$out.='<div align="center"><h3>Alert types</h3></div>';
	
	if ($action=='form') {
		$out.='<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.='
			<form action="index.php" method="post" name="alertTypes">
			<input type="hidden" name="section" value="alertTypes">
			<input type="hidden" name="action" value="add">
			<table border="0" align="center">
				<tr bgcolor="lightblue">
					<td align="center"><B>Description</B></td>
					<td align="center"><B>Delay Before Alarm</B></td>
					<td align="center"><B>Exit Delay</B></td>
					<td align="center"><B>Alarm Duration</B></td>
				</tr>
			';
		$displayedAlertTypes=array();
		$query='SELECT * FROM AlertType ORDER BY Description ASC';
		$res=$securitydbADO->Execute($query);
		while($row=$res->FetchRow()){
			$displayedAlertTypes[]=$row['PK_AlertType'];
			$out.='
				<tr>
					<td align="center"><B>'.$row['Description'].'</B></td>
					<td align="center"><input type="text" name="delay_'.$row['PK_AlertType'].'" value="'.$row['DelayBeforeAlarm'].'"></td>
					<td align="center">'.((in_array($row['PK_AlertType'],array(1,4)))?'<input type="text" name="exitDelay_'.$row['PK_AlertType'].'" value="'.$row['ExitDelay'].'">':'N/A').'</td>
					<td align="center"><input type="text" name="duration_'.$row['PK_AlertType'].'" value="'.$row['AlarmDuration'].'"></td>
				</tr>
			';
		}
		
		$out.='
				<tr>
					<td align="center" colspan="4"><input type="submit" class="button" name="update" value="Update"></td>
				</tr>		
			</table>
			<input type="hidden" name="displayedAlertTypes" value="'.join(',',$displayedAlertTypes).'">
		</form>
		';
	} else {
		$displayedAlertTypesArray=explode(',',$_POST['displayedAlertTypes']);
		foreach($displayedAlertTypesArray as $PK_AlertType){
			$delay=(isset($_POST['delay_'.$PK_AlertType]) && $_POST['delay_'.$PK_AlertType]!='')?@$_POST['delay_'.$PK_AlertType]:NULL;
			$exitDelay=(isset($_POST['exitDelay_'.$PK_AlertType]) && $_POST['exitDelay_'.$PK_AlertType]!='')?$_POST['exitDelay_'.$PK_AlertType]:NULL;
			$duration=(isset($_POST['duration_'.$PK_AlertType]) && $_POST['duration_'.$PK_AlertType]!='')?$_POST['duration_'.$PK_AlertType]:NULL;
			
			$updateAlertType='UPDATE AlertType SET DelayBeforeAlarm=?, ExitDelay=?, AlarmDuration=? WHERE PK_AlertType=?';
			$securitydbADO->Execute($updateAlertType,array($delay,$exitDelay,$duration,$PK_AlertType));
		}
		
		header("Location: index.php?section=alertTypes&msg=Alerts types updated.");
	}
	
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}
?>
