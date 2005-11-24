<?
function ripStatus($output,$mediadbADO,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/powerFile.lang.php');
	
	$powerFileID=getPowerFile($dbADO,(int)@$_SESSION['installationID']);
	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 739';
	

	exec($cmd,$retArray);
	
	// todo: remove test line
	// $retArray=array(0=>'OK:',1=>'158:S1-S,S2-F,S5-R,S10-I,S100-N');
	
	foreach ($retArray AS $line){
		if(strpos($line,'158:')!==false){
			$status=str_replace('158:','',$line);
		}
	}
	if(!isset($status)){
		die('<font color="red">'.$TEXT_ERROR_CANNOT_RETRIEVE_RIP_STATUS_CONST.'</font><br>'.join('<br>',$retArray));
	}
	
	$slotsInfo=get_slots_info($mediadbADO);
	
	$remaining=0;
	if($status==''){
		$out='Status: idle.';
	}else{
		$slotStatusArray=explode(',',$status);
		$out='<table>
				<tr bgColor="lightblue">
					<td align="center"><B>Slot</B></td>
					<td align="center"><B>Disc</B></td>
					<td align="center"><B>Status</B></td>
				</tr>		';
		$pos=0;
		foreach ($slotStatusArray AS $slot){
			$pos++;
			$parts=explode('-',$slot);
			switch ($parts[1]){
				case 'S';
					$slotStatus='completed';
				break;
				case 'F';
					$slotStatus='<span class="err">failed</span>';
				break;
				case 'R';
					$slotStatus='<span class="confirm">ripping</span>';
					$remaining++;
				break;
				case 'N';
					$slotStatus='<span style="color:blue">pending</span>';
					$remaining++;
				break;
				case 'I';
					$slotStatus='<span class="confirm">identifying</span>';
					$remaining++;
				break;				
			}
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
			$index=str_replace('S','',$parts[0]);
			$disc=(isset($slotsInfo[$index]['PK_Disc']))?'<span class="confirm">'.$slotsInfo[$index]['Performer'].' - '.$slotsInfo[$index]['Album'].'</span>':'<span class="err">Not identified</span>';			
			
			$out.='
				<tr bgcolor="'.$color.'">
					<td align="center">'.$parts[0].'</td>
					<td align="center">'.$disc.'</td>
					<td align="center">'.$slotStatus.'</td>
				</tr>';
		}
	}
	$out.='</table>';
	
	if($remaining>0){
		Header("Refresh: 5; url=index.php?section=ripStatus");	
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>