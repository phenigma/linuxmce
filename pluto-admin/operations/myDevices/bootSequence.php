<?
function bootSequence($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = isset($_SESSION['installationID'])?cleanInteger($_SESSION['installationID']):0;
	$computer=isset($_REQUEST['computer'])?cleanInteger($_REQUEST['computer']):0;
 
	if($computer!=0){
		if(isCore($computer,$dbADO))
			$DeviceType='Core';
		elseif(isMediaDirector($computer,$dbADO))
			$DeviceType='MediaDirector';
		else{
			// error case?
		}
		if($DeviceType=='MediaDirector'){
			$getParent='SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device=?';
			$resParent=$dbADO->Execute($getParent,$computer);
			$rowParent=$resParent->FetchRow();
			if($rowParent['FK_Device_ControlledVia']!='')
				if(isCore($rowParent['FK_Device_ControlledVia'],$dbADO))
					$DeviceType='Hybrid';
		}


		switch($DeviceType){
			case 'Core':
				$insertMissingRows='
					INSERT INTO Device_StartupScript 
						(FK_Device, FK_StartupScript, Boot_Order,Background, Enabled, Parameter)
					SELECT '.$computer.', StartupScript.PK_StartupScript, StartupScript.Core_Boot_Order, StartupScript.Core_Background, StartupScript.Core_Enabled, StartupScript.Core_Parameter
						FROM StartupScript
					LEFT JOIN Device_StartupScript ON FK_StartupScript = PK_StartupScript
						AND FK_Device ='.$computer.'
					WHERE FK_Device IS NULL
						ORDER BY Device_StartupScript.Boot_Order
				';
			break;
			case 'MediaDirector':
				$insertMissingRows='
					INSERT INTO Device_StartupScript 
						(FK_Device, FK_StartupScript, Boot_Order,Background, Enabled, Parameter)
					SELECT '.$computer.', StartupScript.PK_StartupScript, StartupScript.MD_Boot_Order, StartupScript.MD_Background, StartupScript.MD_Enabled, StartupScript.MD_Parameter
						FROM StartupScript
					LEFT JOIN Device_StartupScript ON FK_StartupScript = PK_StartupScript
						AND FK_Device ='.$computer.'
					WHERE FK_Device IS NULL
						ORDER BY Device_StartupScript.Boot_Order
				';
			break;
			case 'Hybrid':
				$insertMissingRows='
					INSERT INTO Device_StartupScript 
						(FK_Device, FK_StartupScript, Boot_Order,Background, Enabled, Parameter)
					SELECT '.$computer.', StartupScript.PK_StartupScript, StartupScript.Hybrid_Boot_Order, StartupScript.Hybrid_Background, StartupScript.Hybrid_Enabled, StartupScript.Hybrid_Parameter
						FROM StartupScript
					LEFT JOIN Device_StartupScript ON FK_StartupScript = PK_StartupScript
						AND FK_Device ='.$computer.'
					WHERE FK_Device IS NULL
						ORDER BY Device_StartupScript.Boot_Order
				';
			break;
		}
		$res=$dbADO->Execute($insertMissingRows);
	}	
	
	if ($action == 'form') {
		$out.='
		<form action="index.php" method="POST" name="bootSequence">
		<input type="hidden" name="section" value="bootSequence">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="computer" value="'.$computer.'">	
		<table>
			<tr>
				<td>Change the boot sequence for:</td>
				<td><select name="computer" onChange="javascript:document.bootSequence.action.value=\'form\';document.bootSequence.submit();">
					<option value="0">- Please select -</option>';
		$validComputers=getValidComputersArray($installationID,$dbADO);
		$queryComputers='
			SELECT * FROM Device 
				WHERE PK_Device IN ('.join(',',$validComputers).') AND FK_Installation=?
			ORDER BY Description ASC';
		$resComputers=$dbADO->Execute($queryComputers,$installationID);
		while($rowComputers=$resComputers->FetchRow()){
			$out.='<option value="'.$rowComputers['PK_Device'].'" '.(($rowComputers['PK_Device']==$computer)?'selected':'').'>'.$rowComputers['Description'].'</option>';
		}
		$out.='</select></td>
			</tr>
		</table>';
		if($computer!=0){
			$out.='
		At bootup, this computer will run the following maintenance scripts:
		<table>
			<tr>
				<td align="center"><B>Order</B></td>
				<td align="center"><B>Command</B></td>
				<td align="center"><B>Enabled</B></td>
				<td align="center"><B>Background</B></td>
				<td align="center"><B>Parameters</B></td>
				<td align="center"><B>Configure only</B></td>
			</tr>';

		$selectDeviceStartupScript='
			SELECT * FROM Device_StartupScript
				INNER JOIN StartupScript ON FK_StartupScript=PK_StartupScript
			WHERE
				FK_Device=?
			ORDER BY Device_StartupScript.Boot_Order';
		$displayedSS=array();
		$resDeviceStartupScript=$dbADO->Execute($selectDeviceStartupScript,$computer);
		$lineCount=0;
		while($rowDeviceStartupScript=$resDeviceStartupScript->FetchRow()){
			$displayedSS[]=$rowDeviceStartupScript['FK_StartupScript'];
			$lineCount++;
			$out.='
			<tr bgcolor='.(($lineCount%2==0)?'#F0F3F8':'#FFFFFF').'>
				<td><input type="text" name="order_'.$rowDeviceStartupScript['FK_StartupScript'].'" size="2" value="'.$rowDeviceStartupScript['Boot_Order'].'"></td>
				<td><B>'.$rowDeviceStartupScript['Command'].'</B></td>
				<td align="center"><input type="checkbox" name="enabled_'.$rowDeviceStartupScript['FK_StartupScript'].'" value="1" '.(($rowDeviceStartupScript['Enabled']==0)?'':'checked').'></td>
				<td align="center"><input type="checkbox" name="background_'.$rowDeviceStartupScript['FK_StartupScript'].'" value="1" '.(($rowDeviceStartupScript['Background']!=0)?'checked':'').'></td>
				<td align="center"><input type="text" name="parameters_'.$rowDeviceStartupScript['FK_StartupScript'].'" value="'.$rowDeviceStartupScript['Parameter'].'"></td>
				<td align="center"><input type="checkbox" name="configureOnly" value="1" '.(($rowDeviceStartupScript['ConfigureOnly']!=0)?'checked':'').' disabled></td>
			</tr>
			<tr bgcolor='.(($lineCount%2==0)?'#F0F3F8':'#FFFFFF').'> 
				<td colspan="6">Description: '.$rowDeviceStartupScript['Description'].'</td>
			</tr>
			<tr bgcolor='.(($lineCount%2==0)?'#F0F3F8':'#FFFFFF').'> 
				<td colspan="6">Parameters: '.(($rowDeviceStartupScript['Parameter_Syntax']=='')?'None':$rowDeviceStartupScript['Parameter_Syntax']).'</td>
			</tr>
			<tr bgcolor='.(($lineCount%2==0)?'#F0F3F8':'#FFFFFF').'>
				<td colspan="6">';
			switch($DeviceType){
				case 'Core':
 					$out.='Defaults: Order: '.$rowDeviceStartupScript['Core_Boot_Order'].' Enabled: '.(($rowDeviceStartupScript['Core_Enabled']==1)?'Y':'N').' Background: '.(($rowDeviceStartupScript['Core_Background']==1)?'Y':'N').' Parameters: '.$rowDeviceStartupScript['Core_Parameter'];
 				break;
				case 'MediaDirector':
 					$out.='Defaults: Order: '.$rowDeviceStartupScript['MD_Boot_Order'].' Enabled: '.(($rowDeviceStartupScript['MD_Enabled']==1)?'Y':'N').' Background: '.(($rowDeviceStartupScript['MD_Background']==1)?'Y':'N').' Parameters: '.$rowDeviceStartupScript['MD_Parameter'];
 				break;
				case 'Hybrid':
 					$out.='Defaults: Order: '.$rowDeviceStartupScript['Hybrid_Boot_Order'].' Enabled: '.(($rowDeviceStartupScript['Hybrid_Enabled']==1)?'Y':'N').' Background: '.(($rowDeviceStartupScript['Hybrid_Background']==1)?'Y':'N').' Parameters: '.$rowDeviceStartupScript['Hybrid_Parameter'];
 				break;
			}
 			$out.='</td>
			</tr>			
			';
			}
		}
		$out.='
			<input type="hidden" name="displayedSS" value="'.@join(',',$displayedSS).'">
			<tr> 
				<td colspan="6" align="center"><input type="submit" name="save" value="Save Changes"></td>
			</tr>
		</table>
		</form>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=bootSequence");
			exit(0);
		}
		if(isset($_POST['save'])){
			$displayedSSArray=explode(',',$_POST['displayedSS']);
			foreach($displayedSSArray AS $value){
				$order=@$_POST['order_'.$value];
				$background_=@$_POST['background_'.$value];
				$enabled=@$_POST['enabled_'.$value];
				$parameters=@$_POST['parameters_'.$value];
				$updateDeviceStartupScript='UPDATE Device_StartupScript SET Boot_Order=?, Background=?, Enabled=?, Parameter=? WHERE FK_Device=? AND FK_StartupScript=?';
				$dbADO->Execute($updateDeviceStartupScript,array($order,$background_,$enabled,$parameters,$computer,$value));
			}
			header('Location: index.php?section=bootSequence&computer='.$computer);
		}
		
	}

	$output->setScriptCalendar('null');
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  	
}