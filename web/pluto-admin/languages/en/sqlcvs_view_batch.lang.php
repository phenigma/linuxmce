<?
function sqlcvs_view_batch($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$tablesArray=$dbADO->MetaTables('TABLES');	
		
	if ($action == 'form') {
		$repositories=array();
		for($i=0;$i<count($tablesArray);$i++){
			$table=$tablesArray[$i];
			if(preg_match('/psc_(.*)_tables/',$table)){
				$repositories[str_replace(array('psc_','_tables'),array('',''),$table)]=str_replace(array('psc_','_tables'),array('',''),$table);
			}
		}	
		
		$out.='
		<div class="err" align="center">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="sqlcvs_view_batch">
		<input type="hidden" name="section" value="sqlcvs_view_batch">
		<input type="hidden" name="action" value="rep">	
		
	<div align="center"><h3>sqlCVS View Batch</h3></div>
	<table width="400" cellpadding="3" cellspacing="0">
		<tr>
			<td colspan="3">Choose repository</td>
			<td>'.pulldownFromArray($repositories,'repository','').'</td>
		</tr>
		';
	
	$out.='	
		<tr>
			<td colspan="3">&nbsp;</td>
			<td><input type="submit" class="button" name="submit" value="Next"></td>
		</tr>		
	</table>
	</form>
	<script>
		var frmvalidator = new formValidator("sqlcvs_view_batch");
 		frmvalidator.addValidation("repository","dontselect=0","Please select a repository"); 
	</script>
	';
	} elseif($action=='rep'){
		$repository=$_POST['repository'];
		$out.='		
			<a href="index.php?section=sqlcvs_view_batch">Back</a>
			<table width="600" cellpadding="3" cellspacing="0">
				<tr bgcolor="#DFEBFF">
					<td colspan="5"><B>Repository: '.$repository.'</B></td>
				</tr>
				<tr>
					<td colspan="5">&nbsp;</td>
				</tr>
				<tr bgcolor="#DFEBFF">
					<td width="20"><B>No</B></td>
					<td width="140"><B>Date</B></td>
					<td width="100"><B>IP Address</B></td>
					<td><B>Comment</B></td>
					<td width="50"><B>&nbsp;</B></td>
				</tr>';
		$rs=$dbADO->Execute('SELECT * FROM psc_'.$repository.'_bathdr ORDER BY date DESC');
		$pos=0;
		while($row=$rs->FetchRow()){
			$pos++;
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
		 $out.='
		 	<tr bgcolor="'.$color.'">
				<td width="20">'.$pos.'</td>
				<td width="140">'.date('d-m-Y H:i:s',getUnixStamp($row['date'])).'</td>
				<td width="100">'.$row['IPAddress'].'</td>
				<td>'.$row['comments'].'</td>
		 		<td>'.(($row['PK_psc_'.$repository.'_bathdr']<0)?'<font color="red">Unauthorized</font>':'&nbsp;').'</td>
			</tr>';
		}
		
	}else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=sqlcvs_view_batch&error=You are not authorised to change the installation.");
			exit(0);
		}

		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: sqlCVS View Batch');
	$output->output();
}
?>
