<?
//se iau intai cele selectate, din baza de date si apoi se verifica array-ul din DeviceTemplate_DeviceParameter!
function editAVDevice($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='';
$dbADO->debug=false;

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
$deviceID = (int)$_REQUEST['deviceID'];
	$selectedInstallation = (int)@$_REQUEST['installation'];
	
	if ($deviceID==0) {
		header("Location: index.php?section=userHome");
	}
	
	

if ($action=='form') {
	
	
	
	$queryModels = "SELECT DeviceTemplate.*,DeviceCategory.Description as deviceDescription,
							Manufacturer.Description as manufacturerDescription 
					FROM DeviceTemplate 
						INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
						INNER JOIN Manufacturer ON PK_Manufacturer=FK_Manufacturer 
					WHERE PK_DeviceTemplate='$deviceID'";

	$rs = $dbADO->_Execute($queryModels);	
		while ($row = $rs->FetchRow()) {			
			$deviceTemplateName = $row['Description'];			
			$deviceCategID = $row['FK_DeviceCategory'];
		}
	$rs->Close();
		
	$grabOldValues = "SELECT * FROM DeviceTemplate_AV WHERE FK_DeviceTemplate = ?";
	$resGrabOldValues = $dbADO->Execute($grabOldValues,array($deviceID));
	if ($resGrabOldValues->RecordCount()==0) {
		$insertRecord = "INSERT INTO DeviceTemplate_AV (FK_DeviceTemplate) values(?)";
		$resInsertRecord = $dbADO->Execute($insertRecord,array($deviceID));
		$irPowerDelay = 0;
		$irModeDelay = 0;
		$digitDelay = 0;
		$togglePower = 0;
		$toggleDSP = 0;
		$toggleInput = 0;
		$toggleOutput = 0;
	} else {
		$row=$resGrabOldValues->FetchRow();
		$irPowerDelay = $row['IR_PowerDelay'];
		$irModeDelay = $row['IR_ModeDelay'];
		$digitDelay = $row['DigitDelay'];
		$togglePower = $row['TogglePower'];
		$toggleDSP = $row['ToggleDSP'];
		$toggleInput = $row['ToggleInput'];
		$toggleOutput = $row['ToggleOutput'];
	}
	
		
	
	$dspSelectedTxt='';
	$dspUnselectedTxt='';
	
	$dspSelected=array();
	$dspSelected[]=0;
	$queryDspSelected = "SELECT DeviceTemplate_DSPMode.*,DSPMode.Description as DSP_Desc
							FROM DeviceTemplate_DSPMode 
								INNER JOIN DSPMode on PK_DSPMode = FK_DSPMode							
							WHERE FK_DeviceTemplate='$deviceID' ORDER BY OrderNO ASC";
	$resDspSelected = $dbADO->_Execute($queryDspSelected);
		if ($resDspSelected) {
			while ($row=$resDspSelected->FetchRow()) {				
				$dspSelectedTxt.='<option value="'.$row['FK_DSPMode'].'">'.$row['DSP_Desc'].'</option>';
				$dspSelected[]=$row['FK_DSPMode'];
			}
		}
	$resDspSelected->close();	
	
	
		
	$dspUnselected=array();
	$dspUnselected[]=0;
	
	$queryDsp="SELECT * FROM DSPMode WHERE PK_DSPMode NOT IN (".join(",",$dspSelected).") ORDER BY Description";
	$resDsp= $dbADO->_Execute($queryDsp);
		if ($resDsp) {
			while ($row=$resDsp->FetchRow()) {
				$dspUnselectedTxt.='<option value="'.$row['PK_DSPMode'].'">'.$row['Description'].'</option>';
				$dspUnselected[]=$row['PK_DSPMode'];
			}
		}
	$resDsp->close();
	
	
	$inputSelectedTxt='';
	$inputUnselectedTxt='';
	
	$inputSelected=array();
	$inputSelected[]=0;
	
	$queryInputSelected = "SELECT DeviceTemplate_Input.*,Input.Description as Input_Desc
								FROM DeviceTemplate_Input 	
									INNER JOIN Input on PK_Input = FK_Input
						   WHERE FK_DeviceTemplate='$deviceID' order by OrderNo ASC";
	
	$resInputSelected = $dbADO->_Execute($queryInputSelected);
		if ($resInputSelected) {
			while ($row=$resInputSelected->FetchRow()) {				
				$inputSelectedTxt.='<option value="'.$row['FK_Input'].'">'.$row['Input_Desc'].'</option>';
				$inputSelected[]=$row['FK_Input'];
			}
		}
	$resInputSelected->close();
	
	
	$queryInput="SELECT * FROM Input WHERE PK_Input NOT IN (".join(",",$inputSelected).") ORDER BY Description ASC";
	$resInput= $dbADO->_Execute($queryInput);
		if ($resInput) {
			while ($row=$resInput->FetchRow()) {				
				$inputUnselectedTxt.='<option value="'.$row['PK_Input'].'">'.$row['Description'].'</option>';
				$inputUnselected[]=$row['PK_Input'];
			}
		}
	$resInput->close();
	
	
	$outputSelectedTxt='';
	$outputUnselectedTxt='';
	
	$outputSelected=array();
	$outputSelected[]=0;
	
	$queryOutputSelected = "SELECT DeviceTemplate_Output.*,Output.Description as Output_Desc
								FROM DeviceTemplate_Output 	
									INNER JOIN Output on PK_Output = FK_Output
						   WHERE FK_DeviceTemplate='$deviceID' order by OrderNo ASC";
	
	$resOutputSelected = $dbADO->_Execute($queryOutputSelected);
		if ($resOutputSelected) {
			while ($row=$resOutputSelected->FetchRow()) {				
				$outputSelectedTxt.='<option value="'.$row['FK_Output'].'">'.$row['Output_Desc'].'</option>';
				$outputSelected[]=$row['FK_Output'];
			}
		}
	$resOutputSelected->close();
	
	
	$queryOutput="SELECT * FROM Output WHERE PK_Output NOT IN (".join(",",$outputSelected).") ORDER BY Description ASC";
	$resOutput= $dbADO->_Execute($queryOutput);
		if ($resOutput) {
			while ($row=$resOutput->FetchRow()) {				
				$outputUnselectedTxt.='<option value="'.$row['PK_Output'].'">'.$row['Description'].'</option>';
				$outputUnselected[]=$row['PK_Output'];
			}
		}
	$resOutput->close();
	
	
	$checkedTogglePower = $togglePower>0?"checked":"";
	$checkedDSPModes = $toggleDSP>0?"checked":"";
	$checkedInput = $toggleInput>0?"checked":"";
	$checkedOutput = $toggleOutput>0?"checked":"";


$out.='

<form action="index.php" method="POST" name="editAVDevice">
<input type="hidden" name="section" value="editAVDevice">
<input type="hidden" name="action" value="update">
<input type="hidden" name="deviceID" value="'.$deviceID.'">
<br>';

if(ereg('devices',$from))
	$out.='<div class="err" align="center"><B>WARNING</B>: the changes will affect all devices from <B>'.$deviceTemplateName.'</B> category.</div>';

$out.='
<br>		
<div align="center">
		<b>'.$deviceTemplateName.'</b>
	<br />
		IR Power Delay: <input type="text" name="irPowerDelay" value="'.$irPowerDelay.'" size="5" /><br />
		IR Mode Delay: <input type="text" name="irModeDelay" value="'.$irModeDelay.'" size="5" /><br />
		Digit Delay (ms): <input type="text" name="irDigitDelay" value="'.$digitDelay.'" size="5" /><br /><br />
		Toggle Power: <input type="checkbox" name="toggle_power" value="1" '.$checkedTogglePower.'>  <br />
</div>
	
<br />
			
		<table cellpadding="5" cellspacing="0" border="1" align="center">
		<tr><th>DSP Modes</th><th>Inputs</th><th>Outputs</th></tr>
		<tr>
			<td valign="top">
				<input type="checkbox" value="1" name="toggle_dsp" '.$checkedDSPModes.'/>Toggle (no discrete codes)
				<br />
				<table cellpadding="2" cellspacing="0" border="0">
						<tr>
							<td>
								<select name="DSPMode__available" size="10">									
									'.$dspUnselectedTxt.'
								</select>
							</td>
							<td>
									<input type="button" style="width:40" onclick="moveDualList(this.form.DSPMode__available,this.form.DSPMode__selected,false,this.form.DSPMode__selectedOrdered,this.form.DSPMode__selected)" name=">>"  value="&gt;">
								    <br>
									<input type="button" style="width:40" onclick="moveDualList(this.form.DSPMode__selected,this.form.DSPMode__available,false,this.form.DSPMode__selectedOrdered,this.form.DSPMode__selected)" name="<<"  value="&lt;">
								    <br>       
								    <input type="button" style="width:40" onclick="moveDualList(this.form.DSPMode__available,this.form.DSPMode__selected,true,this.form.DSPMode__selectedOrdered,this.form.DSPMode__selected)"  name=">>>>"  value="&gt;&gt;"> 
								    <br />
								    <input type="button" style="width:40" onclick="moveDualList(this.form.DSPMode__selected,this.form.DSPMode__available,true,this.form.DSPMode__selectedOrdered,this.form.DSPMode__selected)"  name="<<<<"  value="&lt;&lt;">
							</td>
							<td>							
								<select name="DSPMode__selected" size="10">
									'.$dspSelectedTxt.'
								</select>
							</td>
							<td>
								<input type="button" value="U" onClick="move(this.form.DSPMode__selected,this.form.DSPMode__selected.selectedIndex,-1,this.form.DSPMode__selectedOrdered)"><br><br>
								<input type="button" value="D" onClick="move(this.form.DSPMode__selected,this.form.DSPMode__selected.selectedIndex,+1,this.form.DSPMode__selectedOrdered)">
								<input type="hidden" name="DSPMode__selectedOrdered" value="">
							</td>
						</tr>
				</table>
			</td>


			<td valign="top">
				<input type="checkbox" name="toggle_input" value="1" '.$checkedInput.'/>Toggle (no discrete codes)
				<br />
				<table cellpadding="2" cellspacing="0" border="0">
						<tr>
							<td>
								<select name="Input__available" size="10">									
									'.$inputUnselectedTxt.'
								</select>
							</td>
							<td>
									<input type="button" style="width:40" onclick="moveDualList(this.form.Input__available,this.form.Input__selected,false,this.form.Input__selectedOrdered,this.form.Input__selected)" name=">>"  value="&gt;">
								    <br>
									<input type="button" style="width:40" onclick="moveDualList(this.form.Input__selected,this.form.Input__available,false,this.form.Input__selectedOrdered,this.form.Input__selected)" name="<<"  value="&lt;">
								    <br>       
								    <input type="button" style="width:40" onclick="moveDualList(this.form.Input__available,this.form.Input__selected,true,this.form.Input__selectedOrdered,this.form.Input__selected)"  name=">>>>"  value="&gt;&gt;"> 
								    <br />
								    <input type="button" style="width:40" onclick="moveDualList(this.form.Input__selected,this.form.Input__available,true,this.form.Input__selectedOrdered,this.form.Input__selected)"  name="<<<<"  value="&lt;&lt;">
							</td>
							<td>							
								<select name="Input__selected" size="10">
									'.$inputSelectedTxt.'
								</select>
							</td>
							<td>
								<input type="button" value="U" onClick="move(this.form.Input__selected,this.form.Input__selected.selectedIndex,-1,this.form.Input__selectedOrdered)"><br><br>
								<input type="button" value="D" onClick="move(this.form.Input__selected,this.form.Input__selected.selectedIndex,+1,this.form.Input__selectedOrdered)">
								<input type="hidden" name="Input__selectedOrdered" value="">
							</td>
						</tr>
				</table>
			</td>

			<td valign="top">
				<input type="checkbox" name="toggle_output" value="1" '.$checkedOutput.'/>Toggle (no discrete codes)
				<br />
				<table cellpadding="2" cellspacing="0" border="0">
						<tr>
							<td>
								<select name="Output__available" size="10">									
									'.$outputUnselectedTxt.'
								</select>
							</td>
							<td>
									<input type="button" style="width:40" onclick="moveDualList(this.form.Output__available,this.form.Output__selected,false,this.form.Output__selectedOrdered,this.form.Output__selected)" name=">>"  value="&gt;">
								    <br>
									<input type="button" style="width:40" onclick="moveDualList(this.form.Output__selected,this.form.Output__available,false,this.form.Output__selectedOrdered,this.form.Output__selected)" name="<<"  value="&lt;">
								    <br>       
								    <input type="button" style="width:40" onclick="moveDualList(this.form.Output__available,this.form.Output__selected,true,this.form.Output__selectedOrdered,this.form.Output__selected)"  name=">>>>"  value="&gt;&gt;"> 
								    <br />
								    <input type="button" style="width:40" onclick="moveDualList(this.form.Output__selected,this.form.Output__available,true,this.form.Output__selectedOrdered,this.form.Output__selected)"  name="<<<<"  value="&lt;&lt;">
							</td>
							<td>							
								<select name="Output__selected" size="10">
									'.$outputSelectedTxt.'
								</select>
							</td>
							<td>
								<input type="button" value="U" onClick="move(this.form.Output__selected,this.form.Output__selected.selectedIndex,-1,this.form.Output__selectedOrdered)"><br><br>
								<input type="button" value="D" onClick="move(this.form.Output__selected,this.form.Output__selected.selectedIndex,+1,this.form.Output__selectedOrdered)">
								<input type="hidden" name="Output__selectedOrdered" value="">
							</td>
						</tr>
				</table>
			</td>

		</tr>
		</table><br />
		<div align="center"><input type="submit" name="mdl" value="Update" /></div>
		
	</form>
';
//$output->setLeftMenu($leftMenu);

$output->setScriptCalendar('null');

$scriptInHead = '
<SCRIPT LANGUAGE="JavaScript">


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

  if (( srcList.selectedIndex == -1) && (moveAll == false))  {
    return;
  }



  newDestList = new Array( destList.options.length );

  var len = 0;

  for( len = 0; len < destList.options.length; len++ ) {
    if ( destList.options[ len ] != null ) {
      newDestList[ len ] = new Option( destList.options[ len ].text, destList.options[ len ].value, destList.options[ len ].defaultSelected, destList.options[ len ].selected );
    }
  }



  for( var i = 0; i < srcList.options.length; i++ ) { 
    if (srcList.options[i] != null && (srcList.options[i].selected == true || moveAll)) {       
       newDestList[ len ] = new Option( srcList.options[i].text, srcList.options[i].value, srcList.options[i].defaultSelected, srcList.options[i].selected );
       len++;
    }
  }



  // Sort out the new destination list

  //newDestList.sort( compareOptionValues );   // BY VALUES

  //newDestList.sort( compareOptionText );   // BY TEXT



  // Populate the destination with the items from the new array

  for (var j = 0;j < newDestList.length; j++) {
    if ( newDestList[ j ] != null ){
      destList.options[ j ] = newDestList[ j ];
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
  saveOrdered(fieldThatWillBeOrdered,saveToField);
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
</script>

';
$onLoad = "onLoad=saveOrdered(document.forms.editAVDevice.DSPMode__selected,document.forms.editAVDevice.DSPMode__selectedOrdered);saveOrdered(document.forms.editAVDevice.Input__selected,document.forms.editAVDevice.Input__selectedOrdered);saveOrdered(document.forms.editAVDevice.Output__selected,document.forms.editAVDevice.Output__selectedOrdered);";
$output->setScriptInHead($scriptInHead);
$output->setScriptInBody($onLoad);
} else {
	
	$powerDelay = cleanInteger($_POST['irPowerDelay']);
	$modeDelay = cleanInteger($_POST['irModeDelay']);
	$digitDelay = cleanInteger($_POST['irDigitDelay']);
	
	$togglePower = cleanInteger(@$_POST['toggle_power']);
	$toggleDSP = cleanInteger(@$_POST['toggle_dsp']);
	$toggleInput = cleanInteger(@$_POST['toggle_input']);
	$toggleOutput = cleanInteger(@$_POST['toggle_output']);
	
	$dspOrdered=cleanString($_POST['DSPMode__selectedOrdered']);
	$inputOrdered=cleanString($_POST['Input__selectedOrdered']);
	$outputOrdered=cleanString($_POST['Output__selectedOrdered']);
	
	
	
	$updateBasicDetails = "UPDATE DeviceTemplate_AV SET 
			IR_PowerDelay = ?,IR_ModeDelay = ?,DigitDelay =?,TogglePower=?,ToggleDSP=?,ToggleInput=?,ToggleOutput=?
						WHERE  FK_DeviceTemplate = ? ";
	$res = $dbADO->Execute($updateBasicDetails,array($powerDelay,$modeDelay,$digitDelay,$togglePower,
													$toggleDSP,$toggleInput,$toggleOutput,$deviceID)
	);
	
	$dspOrderedArray = explode(",",$dspOrdered);
	$inputOrderedArray = explode(",",$inputOrdered);
	$outputOrderedArray = explode(",",$outputOrdered);
		
	$dspOrderedArray=cleanArray($dspOrderedArray);
	$inputOrderedArray=cleanArray($inputOrderedArray);
	$outputOrderedArray=cleanArray($outputOrderedArray);
	
	//grab id's for dsp
	$dspOrderedArrayIDs=array();
	foreach ($dspOrderedArray as $dspModeElem) {
		list($order,$name) = explode("-",$dspModeElem);
		$query = 'SELECT * FROM DSPMode WHERE Description = ?';		
		$res=$dbADO->Execute($query,array(cleanString($name)));
		if ($res && $res->RecordCount()==1) {
			$row=$res->FetchRow();
			$dspOrderedArrayIDs[]=$row['PK_DSPMode'];
		}
	}
	$dspOrderedArrayIDs[]=0;
	
	//delete dsp that are now unselected 
	$cleanDSPMode = "
		DELETE FROM DeviceTemplate_DSPMode WHERE FK_DeviceTemplate = ? AND FK_DSPMode NOT IN (".join(",",$dspOrderedArrayIDs).")";
	$dbADO->Execute($cleanDSPMode,array($deviceID));
	
	$pos=1;
	foreach ($dspOrderedArrayIDs as $dspModeElem) {
		$checkIfExists='SELECT * FROM DeviceTemplate_DSPMode WHERE FK_DeviceTemplate = ? AND FK_DSPMode = ?';
		$resCheckIfExists = $dbADO->Execute($checkIfExists,array($deviceID,$dspModeElem));
		if ($resCheckIfExists->RecordCount()==1) {
			$updateOrder = "UPDATE DeviceTemplate_DSPMode SET OrderNo = ? WHERE FK_DeviceTemplate= ? AND FK_DSPMode = ? ";
			$resUpdateOrder = $dbADO->Execute($updateOrder,array($pos,$deviceID,$dspModeElem));
		} else {
			if ($dspModeElem!=0) {
				$insertRecord = "INSERT INTO DeviceTemplate_DSPMode (FK_DeviceTemplate,FK_DSPMode,OrderNo) values(?,?,?)";
				$resInsertRecord = $dbADO->Execute($insertRecord,array($deviceID,$dspModeElem,$pos));
			}
		}
		$pos++;	
	}
	
	//grab id's for input
	$inputOrderedArrayIDs=array();
	foreach ($inputOrderedArray as $inputElem) {
		list($order,$name) = explode("-",$inputElem);
		$query = 'SELECT * FROM Input WHERE Description = ?';		
		$res=$dbADO->Execute($query,array(cleanString($name)));
		if ($res && $res->RecordCount()==1) {
			$row=$res->FetchRow();
			$inputOrderedArrayIDs[]=$row['PK_Input'];
		}
	}
	
	$inputOrderedArrayIDs[]=0;
		
	//delete input that are now unselected 
	$cleanInput = "
		DELETE FROM DeviceTemplate_Input WHERE FK_DeviceTemplate = ? AND FK_Input NOT IN (".join(",",$inputOrderedArrayIDs).")
				
	";
	$dbADO->Execute($cleanInput,array($deviceID));
	
	$pos=1;
	foreach ($inputOrderedArrayIDs as $inputElem) {
		$checkIfExists='SELECT * FROM DeviceTemplate_Input WHERE FK_DeviceTemplate = ? AND FK_Input = ?';
		$resCheckIfExists = $dbADO->Execute($checkIfExists,array($deviceID,$inputElem));
		if ($resCheckIfExists->RecordCount()==1) {
			$updateOrder = "UPDATE DeviceTemplate_Input SET OrderNo = ? WHERE FK_DeviceTemplate= ? AND FK_Input = ? ";
			$resUpdateOrder = $dbADO->Execute($updateOrder,array($pos,$deviceID,$inputElem));
		} else {
			if ($inputElem!=0) {
				$insertRecord = "INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Input,OrderNo) values(?,?,?)";
				$resInsertRecord = $dbADO->Execute($insertRecord,array($deviceID,$inputElem,$pos));
			}
		}
		$pos++;	
	}
	
	//grab id's for output
	$outputOrderedArrayIDs=array();
	foreach ($outputOrderedArray as $outputElem) {
		list($order,$name) = explode("-",$outputElem);
		$query = 'SELECT * FROM Output WHERE Description = ?';		
		$res=$dbADO->Execute($query,array(cleanString($name)));
		if ($res && $res->RecordCount()==1) {
			$row=$res->FetchRow();
			$outputOrderedArrayIDs[]=$row['PK_Output'];
		}
	}
	$outputOrderedArrayIDs[]=0;
	
	//delete output that are now unselected 
	$cleanOutput = "
		DELETE FROM DeviceTemplate_Output WHERE FK_DeviceTemplate = ? AND FK_Output NOT IN (".join(",",$outputOrderedArrayIDs).")
			
	";
	$dbADO->Execute($cleanOutput,array($deviceID));
	
	$pos=1;
	foreach ($outputOrderedArrayIDs as $outputElem) {
		$checkIfExists='SELECT * FROM DeviceTemplate_Output WHERE FK_DeviceTemplate = ? AND FK_Output = ?';
		$resCheckIfExists = $dbADO->Execute($checkIfExists,array($deviceID,$outputElem));
		if ($resCheckIfExists->RecordCount()==1) {
			$updateOrder = "UPDATE DeviceTemplate_Output SET OrderNo = ? WHERE FK_DeviceTemplate= ? AND FK_Output = ? ";
			$resUpdateOrder = $dbADO->Execute($updateOrder,array($pos,$deviceID,$outputElem));
		} else {
			if ($outputElem!=0) {
				$insertRecord = "INSERT INTO DeviceTemplate_Output (FK_DeviceTemplate,FK_Output,OrderNo) values(?,?,?)";
				$resInsertRecord = $dbADO->Execute($insertRecord,array($deviceID,$outputElem,$pos));
			}
		}
		$pos++;	
	}
	
	header("Location: index.php?section=editAVDevice&deviceID=$deviceID&from=$from");
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  

}
?>