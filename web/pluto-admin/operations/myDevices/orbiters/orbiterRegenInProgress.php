<?
function orbiterRegenInProgress($output,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/orbiterRegenInProgress.lang.php');
	
	$orbiterID=(int)$_REQUEST['orbiterID'];
	
	if($orbiterID==0){
		return '';
	}
	$orbiterData=$dbADO->Execute('SELECT PK_Orbiter,RegenInProgress,RegenStatus,RegenPercent from Orbiter WHERE PK_Orbiter=?',$orbiterID);
	if($orbiterData->RecordCount()==0){
		return '';
	}
	$row=$orbiterData->FetchRow();
	if($row['RegenInProgress']==1){
		$out='
		<table width="100%">
			<tr>
				<td colspan="2" align="center">'.percentBox($row['RegenPercent']).'</td>
			</tr>		
			<tr>
				<td align="center" colspan="2">'.$TEXT_REGEN_STATUS_CONST.': '.$row['RegenStatus'].'</td>
			</tr>		
			<tr>
				<td colspan="2" align="center"><B><font color="red">'.$TEXT_ORBITER_GENERATION_IN_PROCESS_CONST.'</font></B></td>
			</tr>	
		</table>
		<script>
		function doReload()
		{
			self.location=\'index.php?section=orbiterRegenInProgress&orbiterID='.$orbiterID.'\';
		}
			setTimeout("doReload()",5000);
		</script>
		';
	}else{
		$out='
		<table width="100%">
			<tr>
				<td colspan="2" align="center"><B>'.$TEXT_ORBITER_GENERATION_FINISHED_CONST.'</B></td>
			</tr>	
		</table>
		<script>
		function doReload()
		{
			self.location=\'index.php?section=orbiterRegenInProgress&orbiterID='.$orbiterID.'\';
		}
			setTimeout("doReload()",5000);
		</script>		
		';
	}
	

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function percentBox($percent){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/orbiterRegenInProgress.lang.php');
	
	$out='
	<table width="250" cellpadding="0" cellspacing="0">
		<tr>
			<td width="50">'.$TEXT_PERCENT_CONST.'</td>
			<td bgcolor="green" width="'.(2*$percent).'">&nbsp;</td>
			<td bgcolor="lightblue" width="'.(200-2*$percent).'">&nbsp;</td>
		</tr>
	</table>';
	
	return $out;
}

?>
