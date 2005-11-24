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

		if(isset($_POST['mass_identify'])){
			// mass identity command
			$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 740 157 '.$ripParms;
		}else{
			// mass riping command
			$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 720 157 '.$ripParms;
		}
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
	
	// get powerfile status
	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 703'.$forcedParm;
	exec($cmd,$retArray);
	foreach ($retArray AS $line){
		if(strpos($line,'153:')!==false){
			$status=str_replace('153:','',$line);
		}
	}
	
	// TODO: remove test line
	//$status='D0=1-1,S1=Empty,S2=Empty,S3=Empty,S4=Empty,S5=Full,S6=Empty,S7=Empty,S8=Empty,S9=Empty,S10=Full,S11=Empty,S12=Empty,S13=Empty,S14=Empty,S15=Empty,S16=Empty,S17=Empty,S18=Empty,S19=Empty,S20=Empty,S21=Empty,S22=Empty,S23=Empty,S24=Empty,S25=Empty,S26=Empty,S27=Empty,S28=Empty,S29=Empty,S30=Empty,S31=Empty,S32=Empty,S33=Empty,S34=Empty,S35=Empty,S36=Empty,S37=Empty,S38=Empty,S39=Empty,S40=Empty,S41=Empty,S42=Empty,S43=Empty,S44=Empty,S45=Empty,S46=Empty,S47=Empty,S48=Empty,S49=Empty,S50=Empty,S51=Empty,S52=Empty,S53=Empty,S54=Empty,S55=Empty,S56=Empty,S57=Empty,S58=Empty,S59=Empty,S60=Empty,S61=Empty,S62=Empty,S63=Empty,S64=Empty,S65=Empty,S66=Empty,S67=Empty,S68=Empty,S69=Empty,S70=Empty,S71=Empty,S72=Empty,S73=Empty,S74=Empty,S75=Empty,S76=Empty,S77=Empty,S78=Empty,S79=Empty,S80=Empty,S81=Empty,S82=Empty,S83=Empty,S84=Empty,S85=Empty,S86=Empty,S87=Empty,S88=Empty,S89=Empty,S90=Empty,S91=Empty,S92=Empty,S93=Empty,S94=Empty,S95=Empty,S96=Empty,S97=Empty,S98=Empty,S99=Empty,S100=Empty,S101=Empty,S102=Empty,S103=Empty,S104=Empty,S105=Empty,S106=Empty,S107=Empty,S108=Empty,S109=Empty,S110=Empty,S111=Empty,S112=Empty,S113=Empty,S114=Empty,S115=Empty,S116=Empty,S117=Empty,S118=Empty,S119=Empty,S120=Empty,S121=Empty,S122=Empty,S123=Empty,S124=Empty,S125=Empty,S126=Empty,S127=Empty,S128=Empty,S129=Empty,S130=Empty,S131=Empty,S132=Empty,S133=Empty,S134=Empty,S135=Empty,S136=Empty,S137=Empty,S138=Empty,S139=Empty,S140=Empty,S141=Empty,S142=Empty,S143=Empty,S144=Empty,S145=Empty,S146=Empty,S147=Empty,S148=Empty,S149=Empty,S150=Empty,S151=Empty,S152=Empty,S153=Empty,S154=Empty,S155=Empty,S156=Empty,S157=Empty,S158=Empty,S159=Empty,S160=Empty,S161=Empty,S162=Empty,S163=Empty,S164=Empty,S165=Empty,S166=Empty,S167=Empty,S168=Empty,S169=Empty,S170=Empty,S171=Empty,S172=Empty,S173=Empty,S174=Empty,S175=Empty,S176=Empty,S177=Empty,S178=Empty,S179=Empty,S180=Empty,S181=Empty,S182=Empty,S183=Empty,S184=Empty,S185=Empty,S186=Empty,S187=Empty,S188=Empty,S189=Empty,S190=Empty,S191=Empty,S192=Empty,S193=Empty,S194=Full,S195=Empty,S196=Empty,S197=Empty,S198=Empty,S199=Empty,S200=Empty';
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

function showPowerFileSlots($slotStatus,$mediadbADO){
	$slotInfo=get_slots_info($mediadbADO);
	
	$table1='<table>';
	$table2='<table>';
	$table3='<table>';
	$table4='<table>';	
	for($i=1;$i<=200;$i++){
		if($slotStatus['S'.$i]=='Full'){
			$status=(isset($slotInfo[$i]['PK_Disc']))?'<a href="index.php?section=discAttributes&slot='.$i.'"><span class="confirm">Slot '.$i.': '.$slotInfo[$i]['Performer'].' - '.$slotInfo[$i]['Album'].'</span></a>':'<a href="index.php?section=discAttributes&slot='.$i.'">Slot '.$i.': Not identified</a>';			
		}else{
			$status='<span class="err">Slot '.$i.': Empty</span>';
		}
		
		$color=($i%2==1)?'#F0F3F8':'#FFFFFF';
		$row='
		<tr bgcolor="'.$color.'">
			<td><input type="checkbox" name="slot_'.$i.'" value="1" id="slot_'.$i.'" value="1" '.(($slotStatus['S'.$i]=='Empty')?'disabled':'').' onClick="update_count('.$i.');"></td>
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
			<td colspan="3" align="center">
				<input type="submit" class="button" name="rip" value="Rip selected"> 
				<input type="button" class="button" name="force_refresh" value="Refresh status" onClick="self.location=\'index.php?section=powerFile&forced=1\'">
				<input type="submit" class="button" name="mass_identify" value="Mass identify">
			</td>
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