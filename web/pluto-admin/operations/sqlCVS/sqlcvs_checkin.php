<?
function sqlcvs_checkin($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$tablesArray=$dbADO->MetaTables('TABLES');	
		
	if ($action == 'form') {
		
		$out.='
		<script>
		function selAllCheckboxes(group)
		{
		   eval("val=(document.sqlcvs_checkin.table_"+group+".checked)?true:false");
		   for (i = 0; i < sqlcvs_checkin.elements.length; i++)
		   {
			tmpName=sqlcvs_checkin.elements[i].name;
		     if (sqlcvs_checkin.elements[i].type == "checkbox" && tmpName.indexOf(group+"_")!=-1)
		     {
		         sqlcvs_checkin.elements[i].checked = val;
		     }
		   }
		}
		</script>
				
		<div class="err" align="center">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="sqlcvs_checkin">
		<input type="hidden" name="section" value="sqlcvs_checkin">
		<input type="hidden" name="action" value="add">	
		
	<div align="center"><h3>sqlCVS CheckIn</h3></div>
	<table width="400" cellpadding="3" cellspacing="0">
		<tr>
			<td colspan="3"><B>sqlCVS Host:</B></td>
			<td><input type="text" name="host" value=""></td>
		</tr>
		<tr>
			<td colspan="3"><B>Username:</B></td>
			<td><input type="text" name="username" value=""></td>
		</tr>
		<tr>
			<td colspan="3"><B>Password:</B></td>
			<td><input type="text" name="password" value=""></td>
		</tr>
		<tr>
			<td colspan="3">&nbsp;</td>
			<td><input type="submit" class="button" name="submit" value="Next"></td>
		</tr>		
		';
	

	for($i=0;$i<count($tablesArray);$i++){
		$table=$tablesArray[$i];
		if(preg_match('/psc_(.*)_tables/',$table)){
			$cleanTable=str_replace(array('psc_','_tables'),array('',''),$table);
			$out.='
			<tr bgcolor="#F0F3F8">
				<td>&nbsp;</td>
				<td width="20"><input type="checkbox" name="table_'.$cleanTable.'" value="1" onclick="selAllCheckboxes(\''.$cleanTable.'\');"></td>
				<td colspan="2"><B>'.$cleanTable.'</B></td>
			</tr>';
			$fieldsArray=getAssocArray($table,'PK_'.$table,'Tablename',$dbADO,'','ORDER BY Tablename ASC');
			foreach ($fieldsArray AS $key=>$value){
				$out.='
			<tr>
				<td>&nbsp;</td>
				<td width="20">&nbsp;</td>
				<td width="20"><input type="checkbox" name="'.$cleanTable.'_'.$value.'" value="1"></td>
				<td>'.$value.'</td>
			</tr>';
			}
		}
	}	
	$out.='	
		<tr>
			<td colspan="3">&nbsp;</td>
			<td><input type="submit" class="button" name="submit" value="Next"></td>
		</tr>		
	</table>
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=sqlcvs_checkin&error=You are not authorised to change the installation.");
			exit(0);
		}

		$host=stripslashes($_POST['host']);
		$username=stripslashes($_POST['username']);
		$password=stripslashes($_POST['password']);
		$rParmArray=array();
		$tParmArray=array();
		
		for($i=0;$i<count($tablesArray);$i++){
			$table=$tablesArray[$i];
			if(preg_match('/psc_(.*)_tables/',$table)){
				$cleanTable=str_replace(array('psc_','_tables'),array('',''),$table);
				if(isset($_POST['table_'.$cleanTable])){
					$rParmArray[]=$cleanTable;	
				}
				$fieldsArray=getAssocArray($table,'PK_'.$table,'Tablename',$dbADO,'','ORDER BY Tablename ASC');
				foreach ($fieldsArray AS $key=>$value){
					if(isset($_POST[$cleanTable.'_'.$value])){
						if(!in_array($cleanTable,$rParmArray)){
							$rParmArray[]=$cleanTable;	
						}
						$tParmArray[$cleanTable][]=$value;
					}
				}			
			}
		}
		
		$parmList='';
		foreach ($rParmArray AS $rep){
			$parmList.=' -r '.$rep.' -t '.join(',',$tParmArray[$rep]);
		}
		
		$cmd='/usr/pluto/bin/sqlCVS -H '.$host.' -h localhost -a -n '.$parmList.' -d “'.$username.'” -U "'.$username.'~'.$password.'" -D '.$dbPlutoMainDatabase.' -e checkin';
		
		$out='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
			windowOpen(\'operations/logs/executeLog.php?script=2&command='.urlencode($cmd).'\',\'width=1024,height=768,scrollbars=1,resizable=1\')
			self.location="index.php?section=sqlcvs_checkin";		
		</script>';
		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: sqlCVS CheckIn');
	$output->output();
}
?>
