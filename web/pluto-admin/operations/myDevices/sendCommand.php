<?
function sendCommand($output,$dbADO) {

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID=(int)@$_REQUEST['deviceID'];
	$installationID=(int)@$_SESSION['installationID'];
	if($deviceID==0){
		die('Invalid Device ID specified.');
	}
	$commandID=(int)@$_REQUEST['command'];
	$resParamType=$dbADO->Execute('SELECT * FROM ParameterType');
	$paramType=array();
	while($rowType=$resParamType->FetchRow()){
		$paramType[$rowType['PK_ParameterType']]=$rowType['Description'];
	}
	if ($action == 'form') {
		$selectDevice='
			SELECT Device.Description,PK_Device,DeviceTemplate.Description AS Template, DeviceCategory.Description AS Category,Manufacturer.Description AS Manufacturer, FK_Manufacturer,FK_DeviceCategory
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			WHERE PK_Device=?';
		$resDevice=$dbADO->Execute($selectDevice,$deviceID);
		$rowDevice=$resDevice->FetchRow();
		
		$resCommands=$dbADO->Execute('
			SELECT Command.*
			FROM Command
			INNER JOIN DeviceCommandGroup_Command ON FK_Command=PK_Command
			INNER JOIN DeviceTemplate_DeviceCommandGroup ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup=DeviceCommandGroup_Command.FK_DeviceCommandGroup
			INNER JOIN Device ON Device.FK_DeviceTemplate=DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate
			WHERE PK_Device=?
			ORDER BY Description ASC 
		',$deviceID);
		$commandsPulldown='<select name="command" onChange="document.sendCommand.action.value=\'form\';document.sendCommand.submit();">
			<option value="0">- Please select -</option>';
		while($row=$resCommands->FetchRow()){
			$commandsPulldown.='<option value="'.$row['PK_Command'].'" '.(($row['PK_Command']==$commandID)?'selected':'').'>'.$row['Description'].'</option>';
		}
		$commandsPulldown.='</select>';
		
		$out.='
		<div class="err">'.strip_tags(@$_GET['error']).'</div>
		<div class="confirm"><B>'.stripslashes(@$_GET['msg']).'</B></div>
		<form action="index.php" method="POST" name="sendCommand" enctype="multipart/form-data">
		<input type="hidden" name="section" value="sendCommand">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="action" value="add">	
		<br><br>
			<table>
				<tr>
					<td valign="top" colspan="3">Device <B>'.$rowDevice['Description'].'</B>, # <B>'.$rowDevice['PK_Device'].'</B> <td>
				</tr>
				<tr>
					<td valign="top" colspan="3">Device template <B>'.$rowDevice['Template'].'</B>, category <B>'.$rowDevice['Category'].'</B> and manufacturer <B>'.$rowDevice['Manufacturer'].'</B>.<td>
				</tr>
				<tr>
					<td valign="top" colspan="3">&nbsp;<td>
				</tr>		
				<tr>
					<td colspan="3">Send to this device the command '.$commandsPulldown.'</td>
				</tr>';
		if($commandID!=0){
			$resParams=$dbADO->Execute('
				SELECT Command_CommandParameter.*, FK_ParameterType,CommandParameter.Description AS CP
				FROM Command_CommandParameter 
				INNER JOIN CommandParameter ON FK_CommandParameter=PK_CommandParameter
				WHERE FK_Command=?',$commandID);
			$isOut=0;
			$parmsArray=array();
			while ($rowParam=$resParams->FetchRow()){
				$parmsArray[$rowParam['FK_CommandParameter']]=$rowParam['FK_ParameterType'];
				$isOut+=$rowParam['IsOut'];
			}
			$deliveryMessage=($isOut>0)?'<br>This command has out parameters.  Delivery confirmation is required.<input type="hidden" name="isOut" value="1">':'';
			$out.='
				<tr>
					<td valign="top" colspan="3"><input type="checkbox" name="confirmation" value="1" '.(($isOut>0)?'disabled checked':'').' '.((@$_REQUEST['conf']==1)?'checked':'').'> Request delivery confirmation'.$deliveryMessage.'<td>
				</tr>
				<tr bgcolor="#DDDDDD">
					<td align="center"><B>Param</B></td>
					<td align="center"><B>Data</B></td>
					<td align="center"><B>File</B></td>
				</tr>';		
			$resParams->MoveFirst();	
			while ($rowParam=$resParams->FetchRow()){
				$out.='
				<tr>
					<td align="center"><B># '.$rowParam['FK_CommandParameter'].' '.$rowParam['CP'].' ('.$paramType[$rowParam['FK_ParameterType']].') '.(($rowParam['IsOut']==1)?'(Out)':'').'</B></td>
					<td align="center"><input type="text" name="param_'.$rowParam['FK_CommandParameter'].'"></td>
					<td align="center"><input type="file" name="fileParam_'.$rowParam['FK_CommandParameter'].'"></td>
				</tr>
				<tr>
					<td colspan="3">'.$rowParam['Description'].'</td>
				</tr>';
			}
			$out.='
				<tr bgcolor="#DDDDDD">
					<td  align="center" colspan="3"><input type="submit" class="button" name="send" value="Send Message"> <input type="reset" class="button" name="cancel" value="Cancel"></td>
				</tr>
				<input type="hidden" name="parmsArray" value="'.urlencode(serialize($parmsArray)).'">
				';
		}
		$out.='	
			</table>
			</form>
		</form>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=sendCommand&deviceID=$deviceID&error=You are not authorised to change the installation.");
			exit(0);
		}
		// MessageSend [-r | -o] localhost 0 [device] 1 [command id] [parm1] [value1] [parm2] [value2]
		
		$conf=(isset($_POST['confirmation']) && $_POST['confirmation']==1)?1:0;
		$firstParam=(isset($_POST['confirmation']) && $_POST['confirmation']==1)?'-r':'';
		$firstParam=(isset($_POST['isOut']))?'-o':$firstParam;
		
		$parmsArray=unserialize(urldecode($_POST['parmsArray']));
		$parmsValues='';
		foreach ($parmsArray AS $parm=>$parmType){
			if($parmType==5){
				if($_FILES['fileParam_'.$parm]['name']!=''){
					$prefix='b';
					$value=$_FILES['fileParam_'.$parm]['tmp_name'];
				}else{
					$prefix='D';
					$value=$_POST['param_'.$parm];
				}
			}else{
				if($_FILES['fileParam_'.$parm]['name']!=''){
					$prefix='T';
					$value=$_FILES['fileParam_'.$parm]['tmp_name'];
				}else{
					$prefix='';
					$value=$_POST['param_'.$parm];
				}				
			}			
			$parmsValues.=' '.$prefix.$parm.' "'.$value.'"';
			
		}
		$commandToSend="/usr/pluto/bin/MessageSend $firstParam localhost 0 $deviceID 1 $commandID$parmsValues";		
		exec($commandToSend,$retMessage);
		
		header("Location: index.php?section=sendCommand&deviceID=$deviceID&command=$commandID&conf=$conf&msg=Message Sent succesfully.  <br>$commandToSend<br>Response was:<br>".join('<br>',$retMessage));
		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
