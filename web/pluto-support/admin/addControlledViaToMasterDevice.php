<?php
	//$publicADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)$_REQUEST['deviceID'];
	
	
	
	
	
	if ($action=='form') {
		$alreadySelectedControlledVia = array();
		$alreadySelectedControlledVia[]=$deviceID;
		$querySelectControlledViaForDevice = "
			select MDL_CV.FK_DeviceTemplate_ControlledVia
								FROM DeviceTemplate_DeviceTemplate_ControlledVia MDL_CV
								INNER JOIN  DeviceTemplate MDL1 on FK_DeviceTemplate  = MDL1.PK_DeviceTemplate 
								INNER JOIN DeviceTemplate MDL2 on FK_DeviceTemplate_ControlledVia  = MDL2.PK_DeviceTemplate
								INNER JOIN DeviceCategory DC on MDL2.FK_DeviceCategory = DC.PK_DeviceCategory								
								WHERE MDL_CV.FK_DeviceTemplate = ?
			";
		$rs = $publicADO->Execute($querySelectControlledViaForDevice,array($deviceID));
		while ($row = $rs->FetchRow()) {
			$alreadySelectedControlledVia[]=$row['FK_DeviceTemplate_ControlledVia'];
		}
		$rs->Close();
		$out.='
		<form action="index.php" method="post" name="addControlledViaToMasterDevice">
		<input type="hidden" name="section" value="addControlledViaToMasterDevice">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Controlled via:</td>
					<td>
						'.(@(int)$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID']!=0
								?'You have selected : <b>'.$_SESSION['addControlledViaToMasterDevice']['deviceTemplateName'].'</b>&nbsp;&nbsp;<br />'
								:(
									isset($_GET['msg'])
									?'<b>'.strip_tags($_GET['msg']).'</b>'
									:'')
								 ).' 
								 <input type="text" name="ControlledViaDeviceTemplateID" size="20" value="'.(@(int)$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID']!=0?@$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID']:'').'">
								 <input type="submit" class="button" name="searchDeviceTemplate" value="Search [...]" >
					</td>
				</tr>		
				<tr>
					<td>Reroute Messages To Parent :</td>
					<td>
						<input name="RerouteMessagesToParent" value="1" type="checkbox">
					</td>
				</tr>								 
				<tr>
					<td>Auto Create Children:</td>
					<td>
						<input name="AutoCreateChildren" value="1" type="checkbox">
					</td>
				</tr>					
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addControlledViaToMasterDevice"); 			
		</script>
		';
		
	} else {
		$controlledViaDeviceTemplateID = cleanInteger($_POST['ControlledViaDeviceTemplateID']);
		
		$controlledVia = isset($_POST['controlledVia'])?cleanString($_POST['controlledVia']):'0';
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';		
		$reroute = cleanInteger(@$_POST['RerouteMessagesToParent']);
		$autocreate = cleanInteger(@$_POST['AutoCreateChildren']);
		
		
				
		$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID'] = 0;
		$_SESSION['addControlledViaToMasterDevice']['deviceTemplateName'] = '';
		
			$querySelectRemainingControlledViaForDevice = "
							select  PK_DeviceTemplate,Description from DeviceTemplate 
							where PK_DeviceTemplate = ?";
						$rs = $publicADO->Execute($querySelectRemainingControlledViaForDevice,array($controlledViaDeviceTemplateID));
						while ($row = $rs->FetchRow()) {
								$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID'] = $row['PK_DeviceTemplate'];
								$_SESSION['addControlledViaToMasterDevice']['deviceTemplateName'] = $row['Description'];
						}
			$rs->Close();
			
		if (isset($_POST['searchDeviceTemplate'])) {
			if ($_SESSION['addControlledViaToMasterDevice']['deviceTemplateID'] !=0) {
				header("Location: index.php?section=addControlledViaToMasterDevice&from=$from&deviceID=$deviceID");
				exit();
			} else {
				header("Location: index.php?section=addControlledViaToMasterDevice&from=$from&deviceID=$deviceID&msg=Invalid+ID");
				exit();				
			}
		} 
		
		
		//RerouteMessagesToParent AutoCreateChildren
		
		if ((int)$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID']!=0 && (int)$deviceID!=0) {
			$insertObjToDevice = 'insert into DeviceTemplate_DeviceTemplate_ControlledVia
					(FK_DeviceTemplate, FK_DeviceTemplate_ControlledVia,RerouteMessagesToParent,AutoCreateChildren) 
					values(?,?,?,?)';
						
			$query = $publicADO->Execute($insertObjToDevice,array($deviceID,$_SESSION['addControlledViaToMasterDevice']['deviceTemplateID'],$reroute,$autocreate));
			$lastInsert = $publicADO->Insert_ID();
			
			if ($autocreate==1) {
				$insertPipe = "INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe (FK_DeviceTemplate_DeviceTemplate_ControlledVia) values (?)";
				$query = $publicADO->Execute($insertPipe,array($lastInsert));
				
				header("Location: index.php?section=editControlledViaToMasterDevice&from=$from&objID=$lastInsert");
				exit();
			}
			
			$out.="
			<script>
				alert('Controlled via added!');			
			    opener.location.reload();
				self.close();
			</script>
			";			
		} else {
			header("Location: index.php?section=addControlledViaToMasterDevice&from=$from&deviceID=$deviceID");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
?>
