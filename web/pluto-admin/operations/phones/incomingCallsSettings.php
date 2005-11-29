<?
function incomingCallsSettings($output,$dbADO,$telecomADO,$asteriskADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.=setLeftMenu($dbADO).'
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
		<h3 align="center">Incomming Calls Settings</h3>

		<form action="index.php" method="POST" name="incomingCallsSettings">
			<input type="hidden" name="section" value="incomingCallsSettings">
			<input type="hidden" name="action" value="update">
		
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=incomingCallsSettings&error=You are not authorised to change the installation.");
			exit(0);
		}	
		
		
		header('Location: index.php?section=incomingCallsSettings');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function getPhoneLineDetails($id,$type,$astADO){
	$count=0;
	$res=$astADO->Execute("
		SELECT sip.id,sip.data,sips.data AS sdata, sipp.data AS pdata 
		FROM sip 
		INNER JOIN sip sips ON (sips.id=sip.id) AND (sips.keyword='secret')
		INNER JOIN sip sipp ON (sipp.id=sip.id) AND (sipp.keyword='username')
		WHERE (sip.keyword='account') AND ((sip.data='broadvoice') OR (sip.data='sipgate') OR (sip.data='inphonex'))");
	$out='
	<table align="center" cellpadding="3" cellspacing="0">
		<tr bgcolor="lightblue">
			<td align="center"><B>Type</B></td>
			<td align="center"><B>Data</B></td>
			<td align="center"><B>Password</B></td>
			<td align="center"><B>Username</B></td>
			<td align="center"><B>Actions</B></td>
		</tr>	';
	while($row=$res->FetchRow()){
		$count++;
		$color=($count%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>SIP</td>
			<td>'.$row['data'].'</td>
			<td>'.$row['sdata'].'</td>
			<td>'.$row['pdata'].'</td>
			<td align="center"><a href="index.php?section=incomingCallsSettings">Settings</a> <a href="javascript:if(confirm(\'Are you sure you want to delete this line?\'))self.location=\'index.php?section=phoneLines&action=del&id='.$row['id'].'\'">Del</a></td>
		</tr>';
	}

	$res=$astADO->Execute("
		SELECT iax.id,iax.data,iaxs.data AS sdata, iaxp.data AS pdata 
		FROM iax 
		INNER JOIN iax iaxs ON (iaxs.id=iax.id) AND (iaxs.keyword='secret')
		INNER JOIN iax iaxp ON (iaxp.id=iax.id) AND (iaxp.keyword='username')
		WHERE (iax.keyword='account') AND ((iax.data='fwd') OR (iax.data='teliax') OR (iax.data='efon'))");
	while($row=$res->FetchRow()){
		$count++;
		$color=($count%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>IAX</td>
			<td>'.$row['data'].'</td>
			<td>'.$row['sdata'].'</td>
			<td>'.$row['pdata'].'</td>
			<td align="center"><a href="index.php?section=incomingCallsSettings">Settings</a> <a href="javascript:if(confirm(\'Are you sure you want to delete this line?\'))self.location=\'index.php?section=phoneLines&action=del&id='.$row['id'].'\'">Del</a></td>
		</tr>';
	}
	
	$out.='</table><br>';
	if($count==0){
		$out='';
	}
	
	return $out;
}

?>