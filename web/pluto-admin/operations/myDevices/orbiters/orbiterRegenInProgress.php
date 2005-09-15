<?
function orbiterRegenInProgress($output,$dbADO){
	
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
				<td align="center" colspan="2">Regen status: '.$row['RegenStatus'].'</td>
			</tr>		
			<tr>
				<td colspan="2" align="center"><B><font color="red">Orbiter Generation in process</font></B></td>
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
				<td colspan="2" align="center"><B>Orbiter Generation finished</B></td>
			</tr>	
		</table>
		';
	}
	

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function percentBox($percent){
	$out='
	<table width="250" cellpadding="0" cellspacing="0">
		<tr>
			<td width="50">Percent</td>
			<td bgcolor="green" width="'.(2*$percent).'">&nbsp;</td>
			<td bgcolor="lightblue" width="'.(200-2*$percent).'">&nbsp;</td>
		</tr>
	</table>';
	
	return $out;
}

?>
