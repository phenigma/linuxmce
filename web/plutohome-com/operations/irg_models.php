<?
function irg_models($output)
{
	global $dbPlutoAdminServer,$dbPlutoAdminUser,$dbPlutoAdminPass,$dbPlutoAdminDatabase;
	include_once('include/plutoAdminUtils.inc.php');

	$publicADO = &ADONewConnection('mysql');
	$publicADO->NConnect($dbPlutoAdminServer,urlencode($dbPlutoAdminUser),urlencode($dbPlutoAdminPass),urlencode($dbPlutoAdminDatabase));
	
	/* @var $publicADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$publicADO->debug=false;
	$installationID = 0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$irgID = (int)$_REQUEST['irgID'];

	$userID = (int)@$_SESSION['userID'];
	if ($irgID==0) {
		header("Location: index.php?section=login");
		exit();
	}

	if ($action=='form') {
		$rs=$publicADO->Execute('
			SELECT 
				DISTINCT InfraredGroup_Command.FK_DeviceTemplate,
				DeviceTemplate.Description AS Name,
				Manufacturer.Description AS Manufacturer,
				DeviceCategory.Description AS Category
			FROM InfraredGroup_Command
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			WHERE FK_InfraredGroup=? AND FK_DeviceTemplate IS NOT NULL',$irgID);
		$modelsTable='
		<table cellpadding="3" cellspacing="0" align="center">
			<tr bgcolor="lightblue" class="normaltext">
				<td align="center"><B>Model ID</B></td>
				<td align="center"><B>Description</B></td>
				<td align="center"><B>Manufacturer</B></td>
				<td align="center"><B>Category</B></td>
			</tr>';
		$pos=0;
		while($row=$rs->FetchRow()){
			$pos++;
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
			$modelsTable.='
			<tr class="normaltext" bgcolor="'.$color.'">
				<td align="center">'.$row['FK_DeviceTemplate'].'</td>
				<td align="center"><a href="index.php?section=editAVDevice&deviceID='.$row['FK_DeviceTemplate'].'">'.$row['Name'].'</a></td>
				<td align="center">'.$row['Manufacturer'].'</td>
				<td align="center">'.$row['Category'].'</td>
			</tr>';
		}
		if($rs->RecordCount()==0){
			$modelsTable.='
			<tr class="normaltext">
				<td colspan="4">No models are using this infrared group.</td>
			</tr>';
		}
		$modelsTable.='</table>';
		
		$out.='
	<div class="err"><br>'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
	<h3>Models using infrared group #'.$irgID.'</h3>	
	'.$modelsTable.'
		
		<br><br>
		';	
		
	} else {
		header("Location: index.php?section=irg_models&irgID=$irgID");
	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Models using IR Group # '.$irgID);
	$output->output();
}
?>
