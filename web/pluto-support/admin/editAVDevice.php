<?
/* @var $publicADO ADOConnection */
/* @var $rs ADORecordSet */
$out='';
$publicADO->debug=false;

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
$deviceID = (int)$_REQUEST['deviceID'];
	$selectedInstallation = (int)@$_REQUEST['installation'];
	
	if ($deviceID==0) {
		header("Location: index.php?section=userHome");
	}
	

if ($action=='form') {
	
	$connectorsArray=array();
	$resConnectors=$publicADO->Execute('SELECT * FROM ConnectorType ORDER BY Description ASC');
	while($rowConnectors=$resConnectors->FetchRow()){
		$connectorsArray[$rowConnectors['PK_ConnectorType']]=$rowConnectors['Description'];
	}
	
	
	$queryModels = "SELECT DeviceTemplate.*,DeviceCategory.Description as deviceDescription,
							Manufacturer.Description as manufacturerDescription 
					FROM DeviceTemplate 
						INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
						INNER JOIN Manufacturer ON PK_Manufacturer=FK_Manufacturer 
					WHERE PK_DeviceTemplate='$deviceID'";

	$rs = $publicADO->_Execute($queryModels);	
		while ($row = $rs->FetchRow()) {			
			$deviceTemplateName = $row['Description'];			
			$deviceCategID = $row['FK_DeviceCategory'];
		}
	$rs->Close();
		
	$grabOldValues = "SELECT * FROM DeviceTemplate_AV WHERE FK_DeviceTemplate = ?";
	$resGrabOldValues = $publicADO->Execute($grabOldValues,array($deviceID));
	if ($resGrabOldValues->RecordCount()==0) {
		$insertRecord = "INSERT INTO DeviceTemplate_AV (FK_DeviceTemplate,UsesIR) values(?,1)";
		$resInsertRecord = $publicADO->Execute($insertRecord,array($deviceID));
		$controlSQL='INSERT INTO DeviceTemplate_DeviceCategory_ControlledVia (FK_DeviceTemplate,FK_DeviceCategory) VALUES (?,?)';
		$publicADO->Execute($controlSQL,array($deviceID,$GLOBALS['specialized']));
		$publicADO->Execute($controlSQL,array($deviceID,$GLOBALS['InfraredInterface']));
		
		$irPowerDelay = 0;
		$irModeDelay = 0;
		$digitDelay = 0;
		$togglePower = 0;
		$toggleDSP = 0;
		$toggleInput = 0;
		$toggleOutput = 0;
		$usesIR=1;
	} else {
		$row=$resGrabOldValues->FetchRow();
		$irPowerDelay = $row['IR_PowerDelay'];
		$irModeDelay = $row['IR_ModeDelay'];
		$digitDelay = $row['DigitDelay'];
		$togglePower = $row['TogglePower'];
		$toggleDSP = $row['ToggleDSP'];
		$toggleInput = $row['ToggleInput'];
		$toggleOutput = $row['ToggleOutput'];
		$usesIR = $row['UsesIR'];
	}
	
		
	
	$dspSelectedTxt='';
	$dspUnselectedTxt='';
	
	$dspSelected=array();
	$dspSelected[]=0;
	$queryDspSelected = "SELECT DeviceTemplate_DSPMode.*,Command.Description as DSP_Desc
							FROM DeviceTemplate_DSPMode 
								INNER JOIN Command on PK_Command = FK_Command							
							WHERE FK_DeviceTemplate='$deviceID' ORDER BY OrderNO ASC";
	$resDspSelected = $publicADO->_Execute($queryDspSelected);
		if ($resDspSelected) {
			while ($row=$resDspSelected->FetchRow()) {				
				$dspSelectedTxt.='<option value="'.$row['FK_Command'].'">'.$row['DSP_Desc'].'</option>';
				$dspSelected[]=$row['FK_Command'];
			}
		}
	$resDspSelected->close();	
	
	
		
	$dspUnselected=array();
	$dspUnselected[]=0;
	$jsArray='
		var DSPModeArray=new Array();
		var Input__Array=new Array();
		var Output_Array=new Array();
	';
	$queryDsp="SELECT * FROM Command WHERE FK_CommandCategory=21 ORDER BY Description";
	$ckb=0;
	$resDsp= $publicADO->_Execute($queryDsp);
		if ($resDsp) {
			while ($row=$resDsp->FetchRow()) {
				$onclickFunction='if(document.editAVDevice.DSPMode__available['.$ckb.'].checked)moveDualList(this.form.DSPMode__available,this.form.DSPMode__selected,false,this.form.DSPMode__selectedOrdered,this.form.DSPMode__selected);else eraseElement('.$row['PK_Command'].',this.form.DSPMode__selected);';
				$jsArray.='DSPModeArray['.$row['PK_Command'].']="'.$row['Description'].'";';
				$dspUnselectedTxt.='<input type="checkbox" name="DSPMode__available" value="'.$row['PK_Command'].'" '.((in_array($row['PK_Command'],$dspSelected))?'checked':'').' onClick="'.$onclickFunction.'">'.$row['Description'].'<br>';
				$dspUnselected[]=$row['PK_Command'];
				$ckb++;
			}
		}
	$resDsp->close();
	
	
	$inputSelectedTxt='';
	$inputUnselectedTxt='';
	
	$inputSelected=array();
	$inputSelected[]=0;
	$inputConnectorsArray=array();
	
	$queryInputSelected = "SELECT DeviceTemplate_Input.*,Command.Description as Input_Desc, FK_ConnectorType
								FROM DeviceTemplate_Input 	
									INNER JOIN Command on PK_Command = FK_Command
						   WHERE FK_DeviceTemplate='$deviceID' order by OrderNo ASC";
	
	$resInputSelected = $publicADO->_Execute($queryInputSelected);
		if ($resInputSelected) {
			while ($row=$resInputSelected->FetchRow()) {				
			
				$inputSelectedTxt.='<option value="'.$row['FK_Command'].'">'.$row['Input_Desc'].'</option>';
				$inputSelected[]=$row['FK_Command'];
				$inputConnectorsArray[$row['FK_Command']]=$row['FK_ConnectorType'];
			}
		}
	$resInputSelected->close();

	$inputUnselectedTxt='<table>';
	$queryInput="SELECT * FROM Command WHERE FK_CommandCategory=22 AND PK_Command ORDER BY Description ASC";
	$resInput= $publicADO->_Execute($queryInput);
		if ($resInput) {
			$ckb=0;
			while ($row=$resInput->FetchRow()) {				
				
				$onclickFunction='if(document.editAVDevice.Input__available['.$ckb.'].checked) moveDualList(this.form.Input__available,this.form.Input__selected,false,this.form.Input__selectedOrdered,this.form.Input__selected); else eraseElement('.$row['PK_Command'].',this.form.Input__selected);';
				$jsArray.='Input__Array['.$row['PK_Command'].']="'.$row['Description'].'";';
				$connectorPullDown='<select name="connector_'.$row['PK_Command'].'" style="display:'.((in_array($row['PK_Command'],$inputSelected))?'':'none').';">
					<option value="0">- Please select -</option>';
				foreach ($connectorsArray AS $connectorID=>$connectorDescription){
					$connectorPullDown.='<option value="'.$connectorID.'" '.(($connectorID==@$inputConnectorsArray[$row['PK_Command']])?'selected':'').'>'.$connectorDescription.'</option>';
				}
				$connectorPullDown.='</select>';
				
				$inputUnselectedTxt.='
					<tr>
						<td><input type="checkbox" name="Input__available" value="'.$row['PK_Command'].'" '.((in_array($row['PK_Command'],$inputSelected))?'checked':'').' onClick="'.$onclickFunction.'if(this.checked)showPulldown(\'connector_'.$row['PK_Command'].'\',\'\')">'.$row['Description'].'</td>
						<td>'.$connectorPullDown.'</td>
					</tr>';
				$inputUnselected[]=$row['PK_Command'];
				$ckb++;
			}
		}
	$resInput->close();
	$inputUnselectedTxt.='</table>';
	
	$outputSelectedTxt='';
	$outputUnselectedTxt='';
	
	$outputSelected=array();
	$outputSelected[]=0;
	$outputConnectorsArray=array();
	$queryOutputSelected = "SELECT DeviceTemplate_Output.*,Command.Description as Output_Desc,FK_ConnectorType
								FROM DeviceTemplate_Output 	
									INNER JOIN Command on PK_Command = FK_Command
						   WHERE FK_DeviceTemplate='$deviceID' order by OrderNo ASC";
	
	$resOutputSelected = $publicADO->_Execute($queryOutputSelected);
		if ($resOutputSelected) {
			while ($row=$resOutputSelected->FetchRow()) {				
				$outputSelectedTxt.='<option value="'.$row['FK_Command'].'">'.$row['Output_Desc'].'</option>';
				$outputSelected[]=$row['FK_Command'];
				$outputConnectorsArray[$row['FK_Command']]=$row['FK_ConnectorType'];
			}
		}
	$resOutputSelected->close();
	
	$outputUnselectedTxt='<table>';
	$queryOutput="SELECT * FROM Command WHERE FK_CommandCategory=27 AND PK_Command ORDER BY Description ASC";
	$resOutput= $publicADO->_Execute($queryOutput);
		if ($resOutput) {
			$ckb=0;
			while ($row=$resOutput->FetchRow()) {				
				$onclickFunction='if(document.editAVDevice.Output__available['.$ckb.'].checked) moveDualList(this.form.Output__available,this.form.Output__selected,false,this.form.Output__selectedOrdered,this.form.Output__selected);else eraseElement('.$row['PK_Command'].',this.form.Output__selected);';
				$jsArray.='Output_Array['.$row['PK_Command'].']="'.$row['Description'].'";';
				$connectorPullDown='<select name="connector_'.$row['PK_Command'].'" style="display:'.((in_array($row['PK_Command'],$outputSelected))?'':'none').';">
					<option value="0">- Please select -</option>';
				foreach ($connectorsArray AS $connectorID=>$connectorDescription){
					$connectorPullDown.='<option value="'.$connectorID.'" '.(($connectorID==@$outputConnectorsArray[$row['PK_Command']])?'selected':'').'>'.$connectorDescription.'</option>';
				}
				$connectorPullDown.='</select>';
				
				$outputUnselectedTxt.='
					<tr>
						<td><input type="checkbox" name="Output__available" value="'.$row['PK_Command'].'" '.((in_array($row['PK_Command'],$outputSelected))?'checked':'').' onClick="'.$onclickFunction.'if(this.checked)showPulldown(\'connector_'.$row['PK_Command'].'\',\'\')">'.$row['Description'].'</td>
						<td>'.$connectorPullDown.'</td>
					</tr>';
				$outputUnselected[]=$row['PK_Command'];
				$ckb++;
			}
		}
	$resOutput->close();
	$outputUnselectedTxt.='</table>';
	
	$checkedTogglePower = $togglePower>0?"checked":"";
	$checkedDSPModes = $toggleDSP>0?"checked":"";
	$checkedInput = $toggleInput>0?"checked":"";
	$checkedOutput = $toggleOutput>0?"checked":"";
	$checkedUsesIR = $usesIR>0?"checked":"";
	$enabledIROptions=($usesIR>0)?'':'disabled';
	
	$mediaTypeCheckboxes='';
	$resMT=$publicADO->Execute('
		SELECT MediaType.Description, PK_MediaType, FK_DeviceTemplate 
		FROM MediaType 
		LEFT JOIN DeviceTemplate_MediaType ON FK_MediaType=PK_MediaType AND FK_DeviceTemplate=?
		WHERE DCEaware=0',$deviceID);
	$displayedMT=array();
	$checkedMT=array();
	while($rowMT=$resMT->FetchRow()){
		$displayedMT[]=$rowMT['PK_MediaType'];
		if(!is_null($rowMT['FK_DeviceTemplate']))
			$checkedMT[]=$rowMT['PK_MediaType'];
		$mediaTypeCheckboxes.='<input type="checkbox" name="mediaType_'.$rowMT['PK_MediaType'].'" '.(($rowMT['FK_DeviceTemplate']!='')?'checked':'').'>'.$rowMT['Description'].' ';
		$mediaTypeCheckboxes.=((count($displayedMT))%5==0)?'<br>':'';
	}
$out.='

<form action="index.php" method="POST" name="editAVDevice" onSubmit="setCheckboxes();">
<input type="hidden" name="section" value="editAVDevice">
<input type="hidden" name="action" value="update">
<input type="hidden" name="deviceID" value="'.$deviceID.'">
<input type="hidden" name="displayedMT" value="'.join(',',$displayedMT).'">
<input type="hidden" name="checkedMT" value="'.join(',',$checkedMT).'">
<br>';

if(ereg('devices',$from))
	$out.='<div class="err" align="center"><B>WARNING</B>: the changes will affect all devices from <B>'.$deviceTemplateName.'</B> category.</div>';

$out.='
<br>		
<div align="center">
		<b>'.$deviceTemplateName.'</b>
	<br />
</div>
	
<br />
	<table align="center">
		<tr>
			<td align="right" width="50%">Uses IR: </td>
			<td><input type="checkbox" name="usesIR" value="1" '.$checkedUsesIR.' onclick="document.editAVDevice.action.value=\'update\';setCheckboxes();document.editAVDevice.submit();">
		</td>
		</tr>
		<tr>
			<td colspan="2" align="center"><span class="'.(($usesIR>0)?'':'grayout').'">IR Power Delay: <input type="text" name="irPowerDelay" value="'.$irPowerDelay.'" size="5" '.$enabledIROptions.' /> IR Mode Delay: <input type="text" name="irModeDelay" value="'.$irModeDelay.'" size="5" '.$enabledIROptions.'/> Digit Delay (ms): <input type="text" name="irDigitDelay" value="'.$digitDelay.'" size="5" '.$enabledIROptions.'/></span></td>
		</tr>
		<tr>
			<td align="right"><span class="'.(($usesIR>0)?'':'grayout').'">Toggle Power: </span></td>
			<td><input type="checkbox" name="toggle_power" value="1" '.$checkedTogglePower.' '.$enabledIROptions.' onClick="setCheckboxes();document.editAVDevice.submit();"></td>
		</tr>
		<tr>
			<td align="center" colspan="2"><B>Media Type</B></td>
		</tr>
		<tr>
			<td align="center" colspan="2">'.$mediaTypeCheckboxes.'</td>
		</tr>
	</table>
<div align="center"><input type="submit" class="button" name="mdl" value="Update"  /></div>
		<table cellpadding="5" cellspacing="0" border="1" align="center">
		<tr>
			<th>DSP Modes</th><th>Inputs</th><th>Outputs</th>
		</tr>
		<tr>
			<td valign="top" align="center">
				<input type="checkbox" value="1" name="toggle_dsp" '.$checkedDSPModes.' onClick="setCheckboxes();document.editAVDevice.submit();"/>Toggle (no discrete codes)
				<br />
				<table cellpadding="2" cellspacing="0" border="0">
						<tr>
							<td>
								'.$dspUnselectedTxt.'
							</td>';
			if($toggleDSP>0){
				$out.='
							<td><B>Confirm the order</B><br>
								<select name="DSPMode__selected" size="10">
									'.$dspSelectedTxt.'
								</select>
							</td>
							<td>
								<input type="button" class="button" value="U" onClick="move(this.form.DSPMode__selected,this.form.DSPMode__selected.selectedIndex,-1,this.form.DSPMode__selectedOrdered)"><br><br>
								<input type="button" class="button" value="D" onClick="move(this.form.DSPMode__selected,this.form.DSPMode__selected.selectedIndex,+1,this.form.DSPMode__selectedOrdered)">
							</td>';
			}
			$out.='
					<input type="hidden" name="DSPMode__selectedOrdered" value="">
						</tr>
				</table><br>
				<B>Add new DSP Mode command</B><br><br>
				<input type="text" name="newDSPMode" value=""> <input type="submit" class="button" name="addDSPMode" value="Add">
			</td>


			<td valign="top">
				<input type="checkbox" name="toggle_input" value="1" '.$checkedInput.' onClick="setCheckboxes();document.editAVDevice.submit();"/>Toggle (no discrete codes)
				<br />
				<table cellpadding="2" cellspacing="0" border="0">
						<tr>
							<td>'.$inputUnselectedTxt.'</td>';
			if($toggleInput>0){
				$out.='
							<td valign="top"><B>Confirm the order</B><br>
								<select name="Input__selected" size="10">
									'.$inputSelectedTxt.'
								</select>
							</td>
							<td valign="top">
								<input type="button" class="button" value="U" onClick="move(this.form.Input__selected,this.form.Input__selected.selectedIndex,-1,this.form.Input__selectedOrdered)"><br><br>
								<input type="button" class="button" value="D" onClick="move(this.form.Input__selected,this.form.Input__selected.selectedIndex,+1,this.form.Input__selectedOrdered)">
							</td>
						</tr>';
			}
			$out.='
					<input type="hidden" name="Input__selectedOrdered" value="">
				</table>
				<B>Add new Input command</B><br><br>
				<input type="text" name="newInput" value=""> <input type="submit" class="button" name="addInput" value="Add">
			</td>

			<td valign="top">
				<input type="checkbox" name="toggle_output" value="1" '.$checkedOutput.' onClick="setCheckboxes();document.editAVDevice.submit();"/>Toggle (no discrete codes)
				<br />
				<table cellpadding="2" cellspacing="0" border="0">
						<tr>
							<td valign="top">'.$outputUnselectedTxt.'</td>';
			if($toggleOutput>0){
				$out.='
							<td><B>Confirm the order</B><br>
								<select name="Output__selected" size="10">
									'.$outputSelectedTxt.'
								</select>
							</td>
							<td>
								<input type="button" class="button" value="U" onClick="move(this.form.Output__selected,this.form.Output__selected.selectedIndex,-1,this.form.Output__selectedOrdered)"><br><br>
								<input type="button" class="button" value="D" onClick="move(this.form.Output__selected,this.form.Output__selected.selectedIndex,+1,this.form.Output__selectedOrdered)">
							</td>
						</tr>';
			}
			$out.='
				<input type="hidden" name="Output__selectedOrdered" value="">
				</table>
			<B>Add new Output command</B><br><br>
				<input type="text" name="newOutput" value=""> <input type="submit" class="button" name="addOutput" value="Add">
			</td>

		</tr>
		</table><br />
		<div align="center"><input type="submit" class="button" name="mdl" value="Update"  /></div>
		
	</form>
';
//$output->setLeftMenu($leftMenu);

$output->setScriptCalendar('null');

$scriptInHead = '
<SCRIPT LANGUAGE="JavaScript">

'.$jsArray.'

function showPulldown(pullDownName,value)
{
	eval("document.editAVDevice."+pullDownName+".style.display=\'"+value+"\';");
}


function setCheckboxes()
{
	try{
		if(document.editAVDevice.DSPMode__selectedOrdered.value==""){
			pos=0;
			for(i=0;i<document.editAVDevice.DSPMode__available.length;i++){
				if(document.editAVDevice.DSPMode__available[i].checked){
					document.editAVDevice.DSPMode__selectedOrdered.value+=pos+"-"+DSPModeArray[document.editAVDevice.DSPMode__available[i].value]+",";
					pos++;
				}
			}
			document.editAVDevice.DSPMode__selectedOrdered.value=document.editAVDevice.DSPMode__selectedOrdered.value.substr(0,document.editAVDevice.DSPMode__selectedOrdered.value.length-1)
		}
		
		if(document.editAVDevice.Input__selectedOrdered.value==""){
			pos=0;
			for(i=0;i<document.editAVDevice.Input__available.length;i++){
				if(document.editAVDevice.Input__available[i].checked){
					document.editAVDevice.Input__selectedOrdered.value+=pos+"-"+Input__Array[document.editAVDevice.Input__available[i].value]+",";
					pos++;
				}
			}
			document.editAVDevice.Input__selectedOrdered.value=document.editAVDevice.Input__selectedOrdered.value.substr(0,document.editAVDevice.Input__selectedOrdered.value.length-1)
		}	
	
		if(document.editAVDevice.Output__selectedOrdered.value==""){
			pos=0;
			for(i=0;i<document.editAVDevice.Output__available.length;i++){
				if(document.editAVDevice.Output__available[i].checked){
					document.editAVDevice.Output__selectedOrdered.value+=pos+"-"+Output_Array[document.editAVDevice.Output__available[i].value]+",";
					pos++;
				}
			}
			document.editAVDevice.Output__selectedOrdered.value=document.editAVDevice.Output__selectedOrdered.value.substr(0,document.editAVDevice.Output__selectedOrdered.value.length-1)
		}	
	}catch(e){
		// no commands
	}
}


function compareOptionValues(a, b) { 

  // Radix 10: for numeric values
  // Radix 36: for alphanumeric values

  var sA = parseInt( a.value, 36 );  
  var sB = parseInt( b.value, 36 );  

  return sA - sB;
}



// Compare two options within a list by TEXT

function compareOptionText(a, b) { 

  // Radix 10: for numeric values
  // Radix 36: for alphanumeric values

  var sA = parseInt( a.text, 36 );  
  var sB = parseInt( b.text, 36 );  

  return sA - sB;

}



// Dual list move function

function moveDualList( srcList, destList, moveAll,saveToField,fieldThatWillBeOrdered) {
  // Do nothing if nothing is selected
	try{
		var srcIsCheckbox=(srcList[0].type=="checkbox")?1:0;
	}catch(e){
		var srcIsCheckbox=(srcList.type=="checkbox")?1:0;
	}

	if(srcIsCheckbox==1){

		newDestList = new Array( destList.options.length );
		var len = 0;

		for( len = 0; len < destList.options.length; len++ ) {
			if ( destList.options[ len ] != null ) {
				newDestList[ len ] = new Option( destList.options[ len ].text, destList.options[ len ].value, destList.options[ len ].defaultSelected, destList.options[ len ].selected );
			}
		}
		
		// get source elements
		for( var i = 0; i < srcList.length; i++ ) { 
	    	if ((srcList[i].checked || moveAll) && !inCheckboxArray(srcList[i].value,newDestList)) {    
				try{
					eval("tmpval="+srcList[0].name.substr(0,7)+"Array["+srcList[i].value+"]");
				}catch(e){
					eval("tmpval="+srcList.name.substr(0,7)+"Array["+srcList[i].value+"]");
				}
	       		newDestList[ len ] = new Option( tmpval, srcList[i].value);
	       		len++;
	    	}
	 	}

		// Populate the destination with the items from the new array
		for (var j = 0;j < newDestList.length; j++) {
			if ( newDestList[ j ] != null ){
		    	destList.options[ j ] = newDestList[ j ];
			}
		}

		if(moveAll == true){
			for(i=0;i<srcList.length;i++)
				srcList[i].checked=true;
		}
	}else{
		if (( srcList.selectedIndex == -1) && (moveAll == false))  {
    		return;
		}

		newDestList = new Array( destList.length );
		var len = 0;

		for( len = 0; len < destList.length; len++ ) {
			if ( destList[len].checked ) {
				newDestList[ len ] = destList[len].value;
			}
		}
		
		// get source elements
	  	for( var i = 0; i < srcList.options.length; i++ ) { 
	    	if (srcList.options[i] != null && (srcList.options[i].selected == true || moveAll)) {       
	       		newDestList = deleteFromArray(srcList.options[i].value,newDestList);
	       		len++;
	    	}
	 	}

		// Populate the destination with the items from the new array
		for (var j = 0;j < destList.length; j++) {
			if ( inArray(destList[ j ].value,newDestList) ){
		    	destList[j].checked = true;
			}else{
				destList[j].checked = false;
				try{
					showPulldown(\'connector_\'+destList[ j ].value,\'none\');
				}catch(e){
				}
			}
		}

		// Erase source list selected elements
		for( var i = srcList.options.length - 1; i >= 0; i-- ) { 
	    	if ( srcList.options[i] != null && ( srcList.options[i].selected == true || moveAll ) ) {
	
	       		// Erase Source
	       		//srcList.options[i].value = "";
	       		//srcList.options[i].text  = "";
				srcList.options[i]       = null;
	    	}
		}
	}

	
  // Sort out the new destination list

  //newDestList.sort( compareOptionValues );   // BY VALUES

  //newDestList.sort( compareOptionText );   // BY TEXT

  saveOrdered(fieldThatWillBeOrdered,saveToField);
} 

function eraseElement(value,destList)
{
	// Erase source list selected elements
	for( var i = destList.options.length - 1; i >= 0; i-- ) { 
		if ( destList.options[i].value==value) {
			destList.options[i] = null;
    	}
	}
}

function move(formObj,index,to,saveToField) {

	var list = formObj;
	var total = list.options.length-1;

	if (index == -1) return false;
	if (to == +1 && index == total) return false;
	if (to == -1 && index == 0) return false;

		var items = new Array;
		var values = new Array;
			for (i = total; i >= 0; i--) {
				items[i] = list.options[i].text;
				values[i] = list.options[i].value;
			}
			for (i = total; i >= 0; i--) {
				if (index == i) {
					list.options[i + to] = new Option(items[i],values[i + to], 0, 1);
					list.options[i] = new Option(items[i + to], values[i]);
					i--;
				} else {
					list.options[i] = new Option(items[i], values[i]);
   				}
			}
		//save order into field
	list.focus();
	saveOrdered(list,saveToField);
}

function saveOrdered(listA,saveToFieldA) {
	var theList = "";
	// start with a "?" to make it look like a real query-string
		for (i = 0; i <= listA.options.length-1; i++) { 
			theList += i+\'-\'+listA.options[i].text;//listA.options[i].value
			if (i != listA.options.length-1) theList += ",";
	}
	saveToFieldA.value = theList;   
	//alert(theList);
}
function inCheckboxArray(toFind,targetList)
{
	for(i=0;i<targetList.length;i++){
		if(toFind==targetList[i].value)
			return true;
	}
	return false;
}

function inArray(toFind,targetList)
{
	for(i=0;i<targetList.length;i++){
		if(toFind==targetList[i])
			return true;
	}
	return false;
}

function deleteFromArray(toDelete,targetArray)
{
	retArray=new Array();
	len=0;
	for(i=0;i<targetArray.length;i++){
		if(targetArray[i]!=toDelete){
			retArray[len]=targetArray[i];
			len++;
		}
	}
	return retArray;
}
</script>

';
	$onLoad='onLoad=';
	if($toggleDSP>0)
		$onLoad .= "saveOrdered(document.forms.editAVDevice.DSPMode__selected,document.forms.editAVDevice.DSPMode__selectedOrdered);";
	if($toggleInput>0)
		$onLoad .="saveOrdered(document.forms.editAVDevice.Input__selected,document.forms.editAVDevice.Input__selectedOrdered);";
	if($toggleOutput>0)
		$onLoad.="saveOrdered(document.forms.editAVDevice.Output__selected,document.forms.editAVDevice.Output__selectedOrdered);";
	$output->setScriptInHead($scriptInHead);
	$output->setScriptInBody($onLoad);
} else {
	$powerDelay = cleanInteger(@$_POST['irPowerDelay']);
	$modeDelay = cleanInteger(@$_POST['irModeDelay']);
	$digitDelay = cleanInteger(@$_POST['irDigitDelay']);

	$togglePower = cleanInteger(@$_POST['toggle_power']);
	$toggleDSP = cleanInteger(@$_POST['toggle_dsp']);
	$toggleInput = cleanInteger(@$_POST['toggle_input']);
	$toggleOutput = cleanInteger(@$_POST['toggle_output']);
	$usesIR = cleanInteger(@$_POST['usesIR']);
	
	$dspOrdered=cleanString(@$_POST['DSPMode__selectedOrdered']);
	$inputOrdered=cleanString($_POST['Input__selectedOrdered']);
	$outputOrdered=cleanString($_POST['Output__selectedOrdered']);

	$updateBasicDetails = "UPDATE DeviceTemplate_AV SET 
			IR_PowerDelay = ?,IR_ModeDelay = ?,DigitDelay =?,TogglePower=?,ToggleDSP=?,ToggleInput=?,ToggleOutput=?, UsesIR=?
						WHERE  FK_DeviceTemplate = ? ";
	$res = $publicADO->Execute($updateBasicDetails,array($powerDelay,$modeDelay,$digitDelay,$togglePower,$toggleDSP,$toggleInput,$toggleOutput,$usesIR,$deviceID));
	if($usesIR==1){
		$publicADO->Execute('UPDATE DeviceTemplate_DeviceCategory_ControlledVia SET FK_DeviceCategory=? WHERE FK_DeviceTemplate=? AND FK_DeviceCategory=?',array($GLOBALS['InfraredInterface'],$deviceID,$GLOBALS['rootComputerID']));
	}else{
		$publicADO->Execute('UPDATE DeviceTemplate_DeviceCategory_ControlledVia SET FK_DeviceCategory=? WHERE FK_DeviceTemplate=? AND FK_DeviceCategory=?',array($GLOBALS['rootComputerID'],$deviceID,$GLOBALS['InfraredInterface']));
	}
	
	$dspOrderedArray = explode(",",$dspOrdered);
	$inputOrderedArray = explode(",",$inputOrdered);
	$outputOrderedArray = explode(",",$outputOrdered);
		
	$dspOrderedArray=cleanArray($dspOrderedArray);
	$inputOrderedArray=cleanArray($inputOrderedArray);
	$outputOrderedArray=cleanArray($outputOrderedArray);
	
	$newDSPMode=cleanString($_POST['newDSPMode']);
	$newInput=cleanString($_POST['newInput']);
	$newOutput=cleanString($_POST['newOutput']);

	if($newDSPMode!=''){
		$publicADO->Execute('INSERT INTO Command (Description, FK_CommandCategory) VALUES (?,?)',array($newDSPMode,21));
	}
	if($newInput!=''){
		$publicADO->Execute('INSERT INTO Command (Description, FK_CommandCategory) VALUES (?,?)',array($newInput,22));
	}
	if($newOutput!=''){
		$publicADO->Execute('INSERT INTO Command (Description, FK_CommandCategory) VALUES (?,?)',array($newOutput,27));
	}

	
	//grab id's for dsp
	$dspOrderedArrayIDs=array();
	foreach ($dspOrderedArray as $dspModeElem) {
		list($order,$name) = explode("-",$dspModeElem);
		$query = 'SELECT * FROM Command WHERE FK_CommandCategory=21 AND Description = ?';		
		$res=$publicADO->Execute($query,array(cleanString($name)));
		if ($res && $res->RecordCount()==1) {
			$row=$res->FetchRow();
			$dspOrderedArrayIDs[]=$row['PK_Command'];
		}
	}
	$dspOrderedArrayIDs[]=0;
	
	//delete dsp that are now unselected 
	$cleanDSPMode = "
		DELETE FROM DeviceTemplate_DSPMode WHERE FK_DeviceTemplate = ? AND FK_Command NOT IN (".join(",",$dspOrderedArrayIDs).")";
	$publicADO->Execute($cleanDSPMode,array($deviceID));
	
	$pos=1;
	foreach ($dspOrderedArrayIDs as $dspModeElem) {
		$checkIfExists='SELECT * FROM DeviceTemplate_DSPMode WHERE FK_DeviceTemplate = ? AND FK_Command = ?';
		$resCheckIfExists = $publicADO->Execute($checkIfExists,array($deviceID,$dspModeElem));
		if ($resCheckIfExists->RecordCount()==1) {
			$updateOrder = "UPDATE DeviceTemplate_DSPMode SET OrderNo = ? WHERE FK_DeviceTemplate= ? AND FK_Command = ? ";
			$resUpdateOrder = $publicADO->Execute($updateOrder,array($pos,$deviceID,$dspModeElem));
		} else {
			if ($dspModeElem!=0) {
				$insertRecord = "INSERT INTO DeviceTemplate_DSPMode (FK_DeviceTemplate,FK_Command,OrderNo) values(?,?,?)";
				$resInsertRecord = $publicADO->Execute($insertRecord,array($deviceID,$dspModeElem,$pos));
			}
		}
		$pos++;	
	}
	
	//grab id's for input
	$inputOrderedArrayIDs=array();
	foreach ($inputOrderedArray as $inputElem) {
		list($order,$name) = explode("-",$inputElem);
		$query = 'SELECT * FROM Command WHERE FK_CommandCategory=22 AND Description = ?';		
		$res=$publicADO->Execute($query,array(cleanString($name)));
		if ($res && $res->RecordCount()==1) {
			$row=$res->FetchRow();
			$inputOrderedArrayIDs[]=$row['PK_Command'];
		}
	}
	
	$inputOrderedArrayIDs[]=0;
		
	//delete input that are now unselected 
	$cleanInput = "
		DELETE FROM DeviceTemplate_Input WHERE FK_DeviceTemplate = ? AND FK_Command NOT IN (".join(",",$inputOrderedArrayIDs).")
				
	";
	$publicADO->Execute($cleanInput,array($deviceID));
	
	$pos=1;
	foreach ($inputOrderedArrayIDs as $inputElem) {
		$connectorID=((int)@$_POST['connector_'.$inputElem]!=0)?(int)@$_POST['connector_'.$inputElem]:NULL;
		$checkIfExists='SELECT * FROM DeviceTemplate_Input WHERE FK_DeviceTemplate = ? AND FK_Command = ?';
		$resCheckIfExists = $publicADO->Execute($checkIfExists,array($deviceID,$inputElem));
		if ($resCheckIfExists->RecordCount()==1) {
			$updateOrder = "UPDATE DeviceTemplate_Input SET OrderNo = ?, FK_ConnectorType=? WHERE FK_DeviceTemplate= ? AND FK_Command = ? ";
			$resUpdateOrder = $publicADO->Execute($updateOrder,array($pos,$connectorID,$deviceID,$inputElem));
		} else {
			if ($inputElem!=0) {
				$insertRecord = "INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command,OrderNo,FK_ConnectorType) values(?,?,?,?)";
				$resInsertRecord = $publicADO->Execute($insertRecord,array($deviceID,$inputElem,$pos,$connectorID));
			}
		}
		$pos++;	
	}
	
	//grab id's for output
	$outputOrderedArrayIDs=array();
	foreach ($outputOrderedArray as $outputElem) {
		list($order,$name) = explode("-",$outputElem);
		$query = 'SELECT * FROM Command WHERE FK_CommandCategory=27 AND Description = ?';		
		$res=$publicADO->Execute($query,array(cleanString($name)));
		if ($res && $res->RecordCount()==1) {
			$row=$res->FetchRow();
			$outputOrderedArrayIDs[]=$row['PK_Command'];
		}
	}
	$outputOrderedArrayIDs[]=0;
	
	
	//delete output that are now unselected 
	$cleanOutput = "
		DELETE FROM DeviceTemplate_Output WHERE FK_DeviceTemplate = ? AND FK_Command NOT IN (".join(",",$outputOrderedArrayIDs).")
			
	";
	$publicADO->Execute($cleanOutput,array($deviceID));
	
	$pos=1;
	foreach ($outputOrderedArrayIDs as $outputElem) {
		$connectorID=((int)@$_POST['connector_'.$outputElem]!=0)?(int)@$_POST['connector_'.$outputElem]:NULL;
		$checkIfExists='SELECT * FROM DeviceTemplate_Output WHERE FK_DeviceTemplate = ? AND FK_Command = ?';
		$resCheckIfExists = $publicADO->Execute($checkIfExists,array($deviceID,$outputElem));
		if ($resCheckIfExists->RecordCount()==1) {
			$updateOrder = "UPDATE DeviceTemplate_Output SET FK_ConnectorType=?,OrderNo = ? WHERE FK_DeviceTemplate= ? AND FK_Command = ? ";
			$resUpdateOrder = $publicADO->Execute($updateOrder,array($connectorID,$pos,$deviceID,$outputElem));
		} else {
			if ($outputElem!=0) {
				$insertRecord = "INSERT INTO DeviceTemplate_Output (FK_DeviceTemplate,FK_Command,OrderNo,FK_ConnectorType) values(?,?,?,?)";
				$resInsertRecord = $publicADO->Execute($insertRecord,array($deviceID,$outputElem,$pos,$connectorID));
			}
		}
		$pos++;	
	}
	
	// add or delete records in DeviceTemplate_MediaType
	$displayedMTArray=explode(',',$_POST['displayedMT']);
	$checkedMTArray=explode(',',$_POST['checkedMT']);
	foreach ($displayedMTArray AS $mediaType){
		if(isset($_POST['mediaType_'.$mediaType])){
			if(!in_array($mediaType,$checkedMTArray)){
				$publicADO->Execute('INSERT INTO DeviceTemplate_MediaType (FK_DeviceTemplate,FK_MediaType) VALUES (?,?)',array($deviceID,$mediaType));
			}
		}elseif(in_array($mediaType,$checkedMTArray)){
			$publicADO->Execute('DELETE FROM DeviceTemplate_MediaType WHERE FK_DeviceTemplate=? AND FK_MediaType=?',array($deviceID,$mediaType));
		}
	}
	
	header("Location: index.php?section=editAVDevice&deviceID=$deviceID&from=$from");
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  

?>
