<?
function displayCode($output,$publicADO)
{

	/* @var $dbADO ADOConnection */
	global $PlutoHomeHost;
	$userID=(int)@$_SESSION['userID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$out='<p><b>Infrared code</b>';
	$irgcID=(int)$_REQUEST['irgcID'];
	if($irgcID==0){
		$out.='<p class="normaltext">Invalid code specified.<br><br>';
	}else{
		$data=getFieldsAsArray('InfraredGroup_Command','InfraredGroup.Description AS Description,IRData,Command.Description AS Command',$publicADO,'INNER JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup INNER JOIN Command ON FK_Command=PK_Command WHERE PK_InfraredGroup_Command='.$irgcID);
		if(!isset($data['IRData'])){
			$out.='Specified code doesn\'t have a IR code.';
		}else{
			$out.='<p class="normaltext"><b>Infrared Group:</b> '.$data['Description'][0].'<br>
			<p class="normaltext"> <b>Command: </b>'.$data['Command'][0].'
			<div class="normaltext" style="padding:10px; border-bottom: 1px solid #557DD2; border-top : 1px solid #557DD2; border-left : 1px solid #557DD2; border-right: 1px solid #557DD2;">'.$data['IRData'][0].'</div><br><br>
			<div align="center"><input type="button" class="button" name="button" value="Close" onClick="javascript:self.close();"></div>';
		}
	}
		
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>