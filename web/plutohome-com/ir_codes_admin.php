<?php
require_once('ir_codes_admin/common.php');
require('ir_codes_admin/action_approve_reject_batch.php');
require('ir_codes_admin/action_clone_ir_group.php');
require('ir_codes_admin/action_truncate_batch.php');
require('ir_codes_admin/action_update_ir_group.php');

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'choose';
if($action=='choose'){
	$out=Head($_SESSION['Username'],$_REQUEST['error'],$_REQUEST['msg'],'','',false);
	$out.='
			<tr bgcolor="#DFEBFF">
				<th colspan=2>Choose type of view:</td>
			</tr>
			<tr>
				<td><a href="ir_codes_admin.php?action=repBatch">Batch view</a></td>
				<td><a href="ir_codes_admin.php?action=repIRGroup">IR Groups view</a></td>
			</tr>
		</table>
	</form>';
}elseif($action=='repBatch'){
	$_SESSION['view']='repBatch';
	$out=Head($_SESSION['Username'],$_REQUEST['error'],$_REQUEST['msg'],"ir_codes_admin.php?action=choose");
	$out.='
			<tr bgcolor="#DFEBFF">
				<td width="20"><B>No</B></td>
				<td width="20"><B>Batch ID for IR</B></td>
				<td width="20"><B>Batch ID for DCE</B></td>
				<td width="20"><B>Batch ID for Local</B></td>
				<td align="center" width="140"><B>Date</B></td>
				<td align="center" width="100"><B>IP Address</B></td>
				<td align="center"><B>Comment</B></td>
				<td align="center"><B>Action</B></td>
			</tr>';
	$res=$sqlCVSADO->Execute('SELECT DISTINCT comments,PK_psc_ir_bathdr ID
		FROM psc_ir_bathdr
		INNER JOIN psc_ir_batdet ON FK_psc_ir_bathdr=PK_psc_ir_bathdr
		WHERE PK_psc_ir_bathdr<0 AND FK_psc_ir_bathdr_auth=0 and comments like "IR_%"
		ORDER BY date DESC');
	$rowsIR=$res->GetAssoc();
	$res=$sqlCVSADO->Execute('SELECT DISTINCT comments,PK_psc_dce_bathdr ID
		FROM psc_dce_bathdr
		INNER JOIN psc_dce_batdet ON FK_psc_dce_bathdr=PK_psc_dce_bathdr
		WHERE PK_psc_dce_bathdr<0 AND FK_psc_dce_bathdr_auth=0 and comments like "IR_%"
		ORDER BY date DESC');
	$rowsDCE=$res->GetAssoc();
	$res=$sqlCVSADO->Execute('SELECT DISTINCT comments,PK_psc_local_bathdr ID
		FROM psc_local_bathdr
		INNER JOIN psc_local_batdet ON FK_psc_local_bathdr=PK_psc_local_bathdr
		WHERE PK_psc_local_bathdr<0 AND FK_psc_local_bathdr_auth=0 and comments like "IR_%"
		ORDER BY date DESC');
	$rowsLOCAL=$res->GetAssoc();
	$pos=0;
	$res=$sqlCVSADO->Execute('SELECT irbh.comments,irbh.IPAddress,irbh.date
		FROM psc_ir_bathdr irbh
		INNER JOIN psc_ir_batdet irbd ON irbd.FK_psc_ir_bathdr=irbh.PK_psc_ir_bathdr
		WHERE irbh.PK_psc_ir_bathdr<0 AND irbd.FK_psc_ir_bathdr_auth=0 and comments like "IR_%"
		UNION
		SELECT dcebh.comments,dcebh.IPAddress,dcebh.date
		FROM psc_dce_bathdr dcebh
		INNER JOIN psc_dce_batdet dcebd ON dcebd.FK_psc_dce_bathdr=dcebh.PK_psc_dce_bathdr
		WHERE dcebh.PK_psc_dce_bathdr<0 AND dcebd.FK_psc_dce_bathdr_auth=0 and comments like "IR_%"
		UNION
		SELECT localbh.comments,localbh.IPAddress,localbh.date
		FROM psc_local_bathdr localbh
		INNER JOIN psc_local_batdet localbd ON localbd.FK_psc_local_bathdr=localbh.PK_psc_local_bathdr
		WHERE localbh.PK_psc_local_bathdr<0 AND localbd.FK_psc_local_bathdr_auth=0 and comments like "IR_%"');
	$rows=$res->GetAssoc();
	$comments=array();
	foreach($rows as $comment=>$row){
		$comments[]=$comment;
	}
	$comments=array_unique($comments);
	foreach($comments as $comment){
		$but=array();
		$butTR=array();
		$pos++;
		$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
		$tdIR=fillBut('ir',@$rowsIR[$comment],$comment,$but,$butTR);
		$tdDCE=fillBut('dce',@$rowsDCE[$comment],$comment,$but,$butTR);
		$tdLOCAL=fillBut('local',@$rowsLOCAL[$comment],$comment,$but,$butTR);
		$butTranc=empty($butTR)?'':'&nbsp;
				<input type="button" class="button" name="truncate" value="Trancate" onClick="if(confirm(\'Are you sure you want to truncate these batches?\')) {document.webIR.action.value=\'truncate\';document.webIR.pk.value=\''.join('~',$butTR).'\';document.webIR.pkd.value=\''.$comment.'\';document.webIR.submit();}"">';
			$out.='
			<tr bgcolor="'.$color.'">
				<td width="20">'.$pos.'</td>
				<td width="20">'.$tdIR.'</td>
				<td width="20">'.$tdDCE.'</td>
				<td width="20">'.$tdLOCAL.'</td>
				<td width="140">'.date('d-m-Y H:i:s',getUnixStamp($rows[$comment]['date'])).'</td>
				<td>'.$rows[$comment]['IPAddress'].'</td>
				<td>'.$rows[$comment]['comments'].'</td>
				<td><input type="button" class="button" name="approve" value="Approve All" onclick="if(confirm(\'Are you sure you want to approve these batches?\')) {document.webIR.action.value=\'approve\';document.webIR.pk.value=\''.join('~',$but).'\';document.webIR.submit();}">&nbsp;
				<input type="button" class="button" name="reject" value="Reject All" onClick="if(confirm(\'Are you sure you want to reject these batches?\')) {document.webIR.action.value=\'reject\';document.webIR.pk.value=\''.join('~',$but).'\';document.webIR.submit();}"">'.$butTranc.'</td>
			</tr>';
		}
		$out.='
		</table>
	</form>';
}elseif($action=='repIRGroup'){
	$_SESSION['view']='repIRGroup';
	$out=Head($_SESSION['Username'],$_REQUEST['error'],$_REQUEST['msg'],"ir_codes_admin.php?action=choose");
	$out.='
			<tr bgcolor="#EFFBFF">
				<th colspan="7">Infrared Group</th>
				<th>Action</th>
			</tr>
			<tr bgcolor="#DFEBFF">
				<th width="20"><B>No</td>
				<th width="20"><B>Batch ID for IR</td>
				<th width="20"><B>Batch ID for DCE</td>
				<th width="20"><B>Batch ID for Local</td>
				<th align="center" width="140">Date</td>
				<th align="center" width="100">IP Address</td>
				<th align="center">Comment</td>
				<th align="center">Action</td>
			</tr>';
	$res=$sqlCVSADO->Execute(
'(SELECT DISTINCT Description,PK_InfraredGroup GroupID,psc_batch,bh.comments,bh.IPAddress,bh.date
FROM InfraredGroup_pschist
LEFT JOIN psc_ir_bathdr bh on PK_psc_ir_bathdr=psc_batch
LEFT JOIN psc_ir_batdet bd ON bd.FK_psc_ir_bathdr=psc_batch
WHERE psc_batch<0 AND bd.FK_psc_ir_bathdr_auth=0 and comments like "IR_%")
UNION
(SELECT DISTINCT gr.Description,FK_InfraredGroup GroupID,gc.psc_batch,bh.comments,bh.IPAddress,bh.date
FROM InfraredGroup_Command_pschist gc
LEFT JOIN InfraredGroup gr on gr.PK_InfraredGroup=FK_InfraredGroup
LEFT JOIN psc_ir_bathdr bh on PK_psc_ir_bathdr=gc.psc_batch
LEFT JOIN psc_ir_batdet bd ON bd.FK_psc_ir_bathdr=gc.psc_batch
WHERE gc.psc_batch<0 AND bd.FK_psc_ir_bathdr_auth=0 and comments like "IR_%")
ORDER BY GroupID');
	$rowsGR=$res->GetAssoc();
	$res=$sqlCVSADO->Execute('SELECT DISTINCT comments,PK_psc_dce_bathdr ID
		FROM psc_dce_bathdr
		INNER JOIN psc_dce_batdet ON FK_psc_dce_bathdr=PK_psc_dce_bathdr
		WHERE PK_psc_dce_bathdr<0 AND FK_psc_dce_bathdr_auth=0 and comments like "IR_%"
		ORDER BY date DESC');
	$rowsDCE=$res->GetAssoc();
	$res=$sqlCVSADO->Execute('SELECT DISTINCT comments,PK_psc_local_bathdr ID
		FROM psc_local_bathdr
		INNER JOIN psc_local_batdet ON FK_psc_local_bathdr=PK_psc_local_bathdr
		WHERE PK_psc_local_bathdr<0 AND FK_psc_local_bathdr_auth=0 and comments like "IR_%"
		ORDER BY date DESC');
	$rowsLOCAL=$res->GetAssoc();
	$pos=0;
	foreach($rowsGR as $IRGroup=>$row){
		$but=array();
		$butTR=array();
		$pos++;
		$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
		$color2=($pos%2==0)?'#E0F0F8':'#EFFBFF';
		$tdIR=fillBut('ir',$row['psc_batch'],$row['comments'],$but,$butTR);
		$tdDCE=fillBut('dce',$rowsDCE[$row['comments']],$row['comments'],$but,$butTR);
		$tdLOCAL=fillBut('local',$rowsLOCAL[$row['comments']],$row['comments'],$but,$butTR);
		$butTranc=empty($butTR)?'':'&nbsp;
				<input type="button" class="button" name="truncate" value="Trancate" onClick="if(confirm(\'Are you sure you want to truncate these batches?\')) {document.webIR.action.value=\'truncate\';document.webIR.pk.value=\''.join('~',$butTR).'\';document.webIR.pkd.value=\''.$row['comments'].'\';document.webIR.submit();}"">';
		$out.='
			<tr bgcolor="'.$color2.'">
				<td colspan="7">'.$IRGroup.'</td>
				<td>';
		if(isGoodForTruncClone($row['comments'])){
			$res=$sqlCVSworkADO->Execute('SELECT count(*) from ir_admin_group_history where FK_InfraredGroup='.$row['GroupID']);
			if(($res)&&($row=$res->FetchRow())&&($row[0]=="0"))
				$out.='<input type="button" class="button" name="clone" value="Clone" onClick="if(confirm(\'Are you sure you want to clone this group?\')) {document.webIR.action.value=\'clone\';document.webIR.groupIR.value=\''.$row['GroupID'].'\';document.webIR.pkd.value=\''.$row['comments'].'\';document.webIR.submit();}"">
				&nbsp;<input type="button" class="button" name="update" value="Update" onClick="if(confirm(\'Are you sure you want to Update this group?\')) {document.webIR.action.value=\'update\';document.webIR.groupIR.value=\''.$row['GroupID'].'\';document.webIR.pkd.value=\''.$row['comments'].'\';document.webIR.submit();}"">';
			else
				$out.='<input type="button" class="button" name="update" value="Update" onClick="if(confirm(\'Are you sure you want to Update this group?\')) {document.webIR.action.value=\'update\';document.webIR.groupIR.value=\''.$row['GroupID'].'\';document.webIR.pkd.value=\''.$row['comments'].'\';document.webIR.submit();}"">';
		}else
			$out.='&nbsp;';
		$out.='</td>
			</tr>
			<tr bgcolor="'.$color.'">
				<td width="20">'.$pos.'</td>
				<td width="20">'.$tdIR.'</td>
				<td width="20">'.$tdDCE.'</td>
				<td width="20">'.$tdLOCAL.'</td>
				<td width="140">'.date('d-m-Y H:i:s',getUnixStamp($row['date'])).'</td>
				<td>'.$row['IPAddress'].'</td>
				<td>'.$row['comments'].'</td>
				<td><input type="button" class="button" name="approve" value="Approve All" onclick="if(confirm(\'Are you sure you want to approve these batches?\')){document.webIR.action.value=\'approve\';document.webIR.pk.value=\''.join('~',$but).'\';document.webIR.submit();}">&nbsp;
				<input type="button" class="button" name="reject" value="Reject All" onClick="if(confirm(\'Are you sure you want to reject these batches?\')){document.webIR.action.value=\'reject\';document.webIR.pk.value=\''.join('~',$but).'\';document.webIR.submit();}"">'.$butTranc.'</td>
			</tr>';
		}
		$out.='
		</table>
	</form>';
}elseif($action=='approve'||$action=='reject'){
	$msg=action_approve_reject_batch($action,$_POST['pk']);
	header("Location: ir_codes_admin.php?msg=$msg&action=".$_SESSION['view']);
	exit();
}elseif($action=='clone'||$action=='update'){
	if($action=='clone')
		action_clone_ir_group($_POST['groupIR'],$_POST['pkd']);
	else
		action_update_ir_group($_POST['groupIR'],$_POST['pkd']);
	if(empty($dbSqlCVSworkPass))
		$pass="";
	else
		$pass=" -p \"$dbSqlCVSworkPass\"";
	$cmd=$cmdln.' -h "'.$dbSqlCVSworkServer.'" -u "'.$dbSqlCVSworkUser.$pass.'" -D "'.$dbSqlCVSworkDatabase.'" -r "dce,ir,local" -U "'.$_POST['username'].'~nopass" -c "'.$_POST['pkd'].'-cloned" -a checkin';
//	echo $cmd;
//	exit;
	exec($cmd,$retArray);
	$msg=array_pop($retArray);
	$sqlCVSworkADO->Execute('INSERT into ir_admin_group_history (FK_InfraredGroup)Values('.$_POST['groupIR'].')');
	$msg.=action_approve_reject_batch('reject',$_POST['pk']);
	header("Location: ir_codes_admin.php?msg=$msg&action=".$_SESSION['view']);
	exit();
}elseif($action=='truncate'){
	$batchs=action_truncate_batch($_POST['pkd']);
	if(empty($dbSqlCVSworkPass))
		$pass="";
	else
		$pass=" -p \"$dbSqlCVSworkPass\"";
	$cmd=$cmdln.' -h "'.$dbSqlCVSworkServer.'" -u "'.$dbSqlCVSworkUser.$pass.'" -D "'.$dbSqlCVSworkDatabase.'" -r "dce,ir,local" -U "'.$_POST['username'].'~nopass" -c "'.$_POST['pkd'].'-truncated" -a checkin';
//	echo $cmd;
//	exit;
	exec($cmd,$retArray);
	$msg=array_pop($retArray);
	foreach($batchs as $rep_batchID){
		list($rep,$batchID)=explode('_',$rep_batchID,2);
		$sqlCVSworkADO->Execute("INSERT INTO ir_admin_batch_history (repository, psc_batch)VALUES($rep,$batchID)");
	}
	header("Location: ir_codes_admin.php?msg=$msg&action=".$_SESSION['view']);
	exit();
}
PrintOut($out);

function fillBut($rep,$ID,$comment,&$but,&$butTR){
	global $sqlCVSworkADO;
	if(!empty($ID)){
		$but[]=$rep.'_'.$ID;
		if(isGoodForTruncClone($comment)){
			$res=$sqlCVSworkADO->Execute('SELECT count(*) from ir_admin_batch_history where repository="ir" and psc_batch='.$ID);
			if(($res)&&($row=$res->FetchRow())&&($row[0]=="0"))
				$butTR[]=$rep.'_'.$ID;
		}
		return '<a href="ir_codes_admin/details.php?rep='.$rep.'&batch='.$ID.'" title="Click for '.strtoupper($rep).' batch details">'.$ID.'</a>';
	}else{
		return '&nbsp;';
	}
}

function isGoodForTruncClone($comment){
	$pattern="/-(truncated|cloned)$/";
	return !preg_match($pattern,$comment);
}

?>