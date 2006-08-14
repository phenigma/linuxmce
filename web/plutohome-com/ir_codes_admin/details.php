<?php
require('common.php');

$rep=$_REQUEST['rep'];
$batch=(int)$_REQUEST['batch'];
$out=Head($_SESSION['Username'],@$_REQUEST['error'],@$_REQUEST['msg'],"../ir_codes_admin.php?action=".$_SESSION['view'],'../');
$out.='
			<tr bgcolor="#DFEBFF">
				<td width="20"><B>Batch ID</B></td>
				<td align="center" width="140"><B>Date</B></td>
				<td align="center" width="100"><B>IP Address</B></td>
				<td align="center"><B>Comment</B></td>
				<td align="center"><B>Action</B></td>
			</tr>';
$rs=$sqlCVSADO->Execute('SELECT
	DISTINCT Tablename,
	SUM(New) AS New,
	SUM(Deleted) AS Deleted,
	SUM(Modified) AS Modified,
	PK_psc_'.$rep.'_bathdr ID,date,IPAddress,Comments
FROM psc_'.$rep.'_batdet 
INNER JOIN psc_'.$rep.'_bathdr ON FK_psc_'.$rep.'_bathdr=PK_psc_'.$rep.'_bathdr
WHERE FK_psc_'.$rep.'_bathdr=? AND FK_psc_'.$rep.'_bathdr_auth=0
GROUP BY Tablename
ORDER BY date DESC',$batch);
$pos=0;
$detailsRows='';
while($row=$rs->FetchRow()){
	$pos++;
	if($pos==1){
		$headRow='
			<tr>
				<td width="20">'.$row['ID'].'</td>
				<td width="140">'.date('d-m-Y H:i:s',getUnixStamp($row['date'])).'</td>
				<td>'.$row['IPAddress'].'</td>
				<td>'.$row['Comments'].'</td>
				<td><input type="button" class="button" name="approve" value="Approve" onclick="if(confirm(\'Are you sure you want to approve this batch?\')){document.webIR.action.value=\'approve\';document.webIR.pk.value=\''.$rep.'_'.$row['ID'].'\';document.webIR.submit();}">&nbsp;<input type="button" class="button" name="approuve" value="Reject" onClick="if(confirm(\'Are you sure you want to reject this batch?\')){document.webIR.action.value=\'reject\';document.webIR.pk.value=\''.$rep.'_'.$row['ID'].'\';document.webIR.submit();}"></td>
			</tr>';
	}
	$detailsRows.='
			<tr bgcolor="#DFEBFF">
				<td width="20">'.$pos.'</td>
				<td colspan="4"><B>New: '.$row['New'].', deleted '.$row['Deleted'].', modified '.$row['Modified'].'</B></td>
			</tr>
			<tr>
				<td colspan="5">'.getBatchDetails($batch,$row['Tablename'],$sqlCVSADO,$rep).'</td>
			</tr>';
}
$out.=$headRow.$detailsRows.'
		</table>
	</form>';
PrintOut($out);

function getBatchDetails($batch,$tablename,$sqlCVSADO,$rep){
	$actionsArray=array(0 =>'added',1 =>'deleted',2=>'modified');
	$out='';
	$res=$sqlCVSADO->Execute('SELECT * FROM '.$tablename.'_pschist WHERE psc_batch=?',$batch);
	$out.='<table cellpadding="3" cellspacing="0" class="normaltext" width="100%">
		<tr bgcolor="#DFEBFF">
			<td width="20">&nbsp;</td>
			<td width="20" align="center"><B>Pos</B></td>
			<td align="center"><B>psc_id</B></td>
			<td align="center"><B>Description</B></td>
			<td align="center"><B>psc_toc</B></td>
			<td align="center"><B>Date</B></td>
		</tr>';
	$pos=0;
	while($row=$res->FetchRow()){
		$pos++;
		$td='<td align="center"><a href="detailsrow.php?msg='.$msg.'&batch='.$batch.'&rep='.$rep.'&tablename='.$tablename.'&pkd='.$row['psc_id'].'&type='.$row['psc_toc'].'" class="normal_row">'.$row['psc_id'].'</a></td>';
		$out.='
		<tr bgcolor="#EFF5FF">
			<td width="20">&nbsp;</td>
			<td width="20" align="center">'.$pos.'</td>
			'.$td.'
			<td align="center">'.@$row['Description'].'</td>
			<td align="center">'.$actionsArray[$row['psc_toc']].'</td>
			<td align="center">'.date('d-m-Y H:i:s',mysql_to_unix_timestamp($row['psc_mod'])).'</td>
		</tr>
		<tr>
			<td colspan="6" height="2"></td>
		</tr>';
	}
	$out.='
	</table>';
	return $out;
}

?>