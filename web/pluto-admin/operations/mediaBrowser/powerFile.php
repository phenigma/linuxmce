<?
function powerFile($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/powerFile.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$slotStatus=getSlotStatus($dbADO,(int)@$_REQUEST['forced']);
	
	
	if($action=='form'){
				
	$scriptInHead='
		<script>
		var slots_checked=0;
		function selAllCheckboxes()
		{
		   val=(document.powerFile.sel_all.checked)?true:false;
		   for (i = 0; i < powerFile.elements.length; i++)
		   {
		     if (powerFile.elements[i].type == "checkbox" && powerFile.elements[i].disabled!=true)
		     {
		         powerFile.elements[i].checked = val;
				 if(val==true){
					slots_checked++;
				 }else{
				 	slots_checked--;
				 }
		     }
		   }
		}
	
		function validateForm(){
			if(slots_checked==0){
				alert("Please select at least a slot.");
				return false;
			}
	
			return true;
		}
	
		function update_count(val){
			eval(\'selected_ckb=document.getElementById("slot_\'+val+\'")\');
			if(selected_ckb.disabled==false){
				slots_checked=(selected_ckb.checked)?slots_checked+1:slots_checked-1
			}
		}
		</script>';		
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<h3 align="center">Power File</h3>
		<form action="index.php" method="POST" name="powerFile" onSubmit="return validateForm();">
			<input type="hidden" name="section" value="powerFile">
			<input type="hidden" name="action" value="rip">
			'.showPowerFileSlots($slotStatus,$mediadbADO).'
		</form>';
	}elseif($action=='rip'){
		$checkedSlots=get_checked_slots();
		$ripParms=join(',',$checkedSlots);
		
		$powerFileID=getPowerFile($dbADO,(int)@$_SESSION['installationID']);
		$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 720 157'.$ripParms;
		exec($cmd);
		$out='
		<B>Ripping status: <span class="confirm">'.$cmd.'</span></B>
		<iframe src="index.php?section=ripStatus" style="width:100%;height:60%;border:0;"></iframe>
		';
		
	}else{
		
		$suffix=(isset($error))?'&error='.$error:'&msg='.@$msg;
		header('Location: index.php?section=powerFile'.$suffix);
	}
	
	$output->setScriptInHead(@$scriptInHead);	
	$output->setNavigationMenu(array($TEXT_POWERFILE_CONST=>'index.php?section=powerFile'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_POWERFILE_CONST);
	$output->output();
}

function getSlotStatus($dbADO,$forced=0){
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/powerFile.lang.php');
		
	$powerFileID=getPowerFile($dbADO,(int)@$_SESSION['installationID']);
	$forcedParm=($forced==0)?'':' 21 1';
	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 703'.$forcedParm;
	exec($cmd,$retArray);
	foreach ($retArray AS $line){
		if(strpos($line,'153:')!==false){
			$status=str_replace('153:','',$line);
		}
	}
	if(!isset($status)){
		error_redirect($TEXT_ERROR_CANNOT_RETRIEVE_STATUS_CONST,'index.php?section=powerFile');
	}

	$arr=explode(',',$status);
	$statArray=array();
	foreach ($arr AS $item){
		if(strpos($item,'D0')!==false || strpos($item,'D1')!==false || strpos($item,'D2')!==false){
			array_shift($arr);
		}else{
			$parts=explode('=',$item);
			$statArray[$parts[0]]=$parts[1];
		}
	}

	return $statArray;
}

function get_slots_info($mediadbADO){
	// hard-coded to extract performer and album
	$slotInfo=array();
	$res=$mediadbADO->Execute('
		SELECT PK_Disc,Slot, A1.Name AS Performer,DA1.FK_Attribute, DA2.FK_Attribute,A2.Name AS Album
		FROM Disc 
		LEFT JOIN Disc_Attribute DA1 ON DA1.FK_Disc=PK_Disc 
		INNER JOIN Attribute A1 ON DA1.FK_Attribute=A1.PK_Attribute AND A1.FK_AttributeType=2 
		LEFT JOIN Disc_Attribute DA2 ON DA2.FK_Disc=PK_Disc 
		INNER JOIN Attribute A2 ON DA2.FK_Attribute=A2.PK_Attribute AND A2.FK_AttributeType=3
		WHERE DA1.Track=0 AND DA2.Track=0
		');
	while($row=$res->FetchRow()){
		$slotInfo[$row['Slot']]['Performer']=$row['Performer'];
		$slotInfo[$row['Slot']]['Album']=$row['Album'];
		$slotInfo[$row['Slot']]['PK_Disc']=$row['PK_Disc'];
	}
	
	return $slotInfo;
}

function showPowerFileSlots($slotStatus,$mediadbADO){
	$slotInfo=get_slots_info($mediadbADO);
	
	$table1='<table>';
	$table2='<table>';
	$table3='<table>';
	$table4='<table>';	
	for($i=1;$i<=200;$i++){
		if($slotStatus['S'.$i]=='Full'){
			$status=(isset($slotInfo[$i]['PK_Disc']))?'<a href="index.php?section=discAttributes&slot='.$i.'"><span class="confirm">Slot '.$i.': '.$slotInfo[$i]['Performer'].' - '.$slotInfo[$i]['Album'].'</span></a>':'<span class="err">Slot '.$i.': Not identified</span>';			
		}else{
			$status='<span class="err">Slot '.$i.': Empty</span>';
		}
		
		$color=($i%2==1)?'#F0F3F8':'#FFFFFF';
		$row='
		<tr bgcolor="'.$color.'">
			<td><input type="checkbox" name="slot_'.$i.'" value="1" id="slot_'.$i.'" value="1" '.(($slotStatus['S'.$i]=='Empty' || !isset($slotInfo[$i]['PK_Disc']))?'disabled':'').' onClick="update_count('.$i.');"></td>
			<td>'.$status.'</td>
		</tr>';
		if($i<=50){
			$table1.=$row;
		}elseif($i<=100){
			$table2.=$row;
		}elseif($i<=150){
			$table3.=$row;
		}else{
			$table4.=$row;
		}
	}
	$table1.='</table>';
	$table2.='</table>';
	$table3.='</table>';
	$table4.='</table>';

	// TODO: add mass identify button and command
	$out='
	<table align="center">
		<tr>
			<td>'.$table1.'</td>
			<td>'.$table2.'</td>
			<td>'.$table3.'</td>
			<td>'.$table4.'</td>
		</tr>
		<tr>
			<td><input type="checkbox" name="sel_all" value="1" onClick="selAllCheckboxes()"> Check/uncheck all</td>
			<td colspan="3" align="center"><input type="submit" class="button" name="rip" value="Rip selected"> <input type="button" class="button" name="force_refresh" value="Refresh status" onClick="self.location=\'index.php?section=powerFile&forced=1\'"></td>
		</tr>	
	</table>';
	
	return $out;
}

function get_checked_slots(){
	$checkedSlots=array();
	for($i=1;$i<201;$i++){
		if(isset($_REQUEST['slot_'.$i]) && $_REQUEST['slot_'.$i]==1){
			$checkedSlots[]=$i;
		}
	}
	
	return $checkedSlots;
}
?>