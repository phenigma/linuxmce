<?php
require('common.php');

$highlight=$old=false;
$res=$sqlCVSADO->Execute('SELECT * FROM '.$_REQUEST['tablename'].'_pschist WHERE psc_id='.$_REQUEST['pkd'].' and psc_batch='.$_REQUEST['batch']);
$out=Head($_SESSION['Username'],@$_REQUEST['error'],@$_REQUEST['msg'],'details.php?msg='.$msg.'&batch='.$_REQUEST['batch'].'&rep='.$_REQUEST['rep'],'../',false);
if($row=$res->FetchRow()){
	switch($_REQUEST['type']){
		case "0":
			break;
		case "1":
			break;
		case "2":
			$res2=$sqlCVSADO->Execute('SELECT * FROM '.$_REQUEST['tablename'].'_pschmask WHERE psc_id='.$_REQUEST['pkd'].' AND psc_batch='.$_REQUEST['batch']);
			if($mask=$res2->FetchRow()){
				$highlight=true;
			}
			if(!empty($row['PK_'.$_REQUEST['tablename']])){
				$res3=$sqlCVSADO->Execute('SELECT * FROM '.$_REQUEST['tablename'].' WHERE PK_'.$_REQUEST['tablename'].'='.$row['PK_'.$_REQUEST['tablename']]);
				if($oldrow=$res3->FetchRow())
					$old=true;
			}
	}
	$pos=$posfield=0;
	foreach($row as $col=>$val){
		$posfield++;
		if($posfield%2==1)continue;
		if($highlight&&$mask[$pos]==1){
			$color=($pos%2==0)?'#F0A3A8':'#FFAFAF';
			$color2=($pos%2==0)?'#DF9BAF':'#E79FAF';
		}else{
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
			$color2=($pos%2==0)?'#DFEBFF':'#E7EFFF';
		}
		$tdOld=$old?'
				<td>&nbsp;</td>
				<td bgcolor="'.$color.'"><pre>'.$oldrow[$pos].'</pre></td>':'';
		$out.='
			<tr>
				<td bgcolor="'.$color2.'" align="center"><B>'.$col.'</B></td>
				<td bgcolor="'.$color.'"><pre>'.$val.'</pre></td>'.$tdOld.'
			</tr>';
		$pos++;
	}
}
$out.='
		</table>
	</form>';
PrintOut($out);

?>