<?
function sqlcvs_diff($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='
	<script>
	function windowOpen(locationA,attributes) {
		window.open(locationA,\'\',attributes);
	}
	</script>
	';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$tablesArray=$dbADO->MetaTables('TABLES');	
		
	if ($action == 'form') {
		
		$out.='
		<script>
		function selAllCheckboxes(group)
		{
		   eval("val=(document.sqlcvs_diff.table_"+group+".checked)?true:false");
		   for (i = 0; i < sqlcvs_diff.elements.length; i++)
		   {
			tmpName=sqlcvs_diff.elements[i].name;
		     if (sqlcvs_diff.elements[i].type == "checkbox" && tmpName.indexOf(group+"_")!=-1)
		     {
		         sqlcvs_diff.elements[i].checked = val;
		     }
		   }
		}
		</script>
				
		<div class="err" align="center">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="sqlcvs_diff">
		<input type="hidden" name="section" value="sqlcvs_diff">
		<input type="hidden" name="action" value="choose">	
		
	<div align="center"><h3>sqlCVS Diff</h3></div>
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
	} elseif($action=='choose'){
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
		
		$cmd='/usr/pluto/bin/sqlCVS -H '.$host.' -h localhost -a -n '.$parmList.' -d “'.$username.'” -U "'.$username.'~'.$password.'" -D '.$dbPlutoMainDatabase.' -e -f /tmp/tmp_sqlcvs_file diff';
		exec($cmd,$retArray,$retVal);
		
		$out.='
	<form action="index.php" method="POST" name="sqlcvs_diff">
		<input type="hidden" name="section" value="sqlcvs_diff">
		<input type="hidden" name="action" value="finish">	
		
		<table>
		<tr>
			<td><B>sqlCVS Host:</B></td>
			<td><input type="text" name="host" value="'.$host.'"></td>
		</tr>
		<tr>
			<td><B>Username:</B></td>
			<td><input type="text" name="username" value="'.$username.'"></td>
		</tr>
		<tr>
			<td><B>Password:</B></td>
			<td><input type="text" name="password" value="'.$password.'"></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td><input type="submit" class="button" name="submit" value="Next"></td>
		</tr>
		</table>
		<table width="600" cellpadding="3" cellspacing="0">';
		$repository='';
		$table='';
		if($retVal==0){
			$resultArray=file('/tmp/tmp_sqlcvs_file');
			$lineNo=0;
			foreach ($resultArray AS $line){
				$items=explode("\t",$line);
				$lineRep=str_replace('REP:','',$items[0]);
				if($lineRep!=$repository){
					$repository=$lineRep;
					$out.='
					<tr bgcolor="#F0F3F8">
						<td colspan="5"><B>Repository: '.$lineRep.'</B></td>
					</tr>';
				}
				$lineTable=str_replace('TABLE:','',$items[1]);
				if($lineTable!=$table){
					$table=$lineTable;
					$out.='
					<tr>
						<td colspan="5"><B>Table: '.$lineTable.'</B></td>
					</tr>';
					$cols=array_values($dbADO->MetaColumnNames($lineTable));
					$out.='
					<tr>
						<td width="20">&nbsp;</td>
						<td width="20">&nbsp;</td>
						<td bgcolor="#F0F3F8">'.$cols[0].'</td>
						<td bgcolor="#F0F3F8">'.$cols[1].'</td>
						<td bgcolor="#F0F3F8">'.$cols[2].'</td>
					</tr>';
					
				}

				$lineValues=getFieldsAsArray($lineTable,$cols[0].','.$cols[1].','.$cols[2],$dbADO,str_replace('WHERE:','',$items[4]));
				$out.='
					<tr>
						<td><input type="checkbox" name="line_'.$lineNo.'" value="1"></td>';
				if(str_replace('CHANGE:','',$items[3])!='DEL'){
					$out.='
						<td><a href="javascript:windowOpen(\'showRecord.php?table='.$lineTable.'&where='.str_replace('WHERE:','',$items[4]).'\',\'width=800,height=400,scrollbars=1,resizable=1\');">'.str_replace('CHANGE:','',$items[3]).'</a></td>
						<td>'.substr($lineValues[$cols[0]][0],0,20).'</td>
						<td>'.substr($lineValues[$cols[1]][0],0,20).'</td>
						<td>'.substr($lineValues[$cols[2]][0],0,20).'</td>
					';
				}else{
					$out.='
						<td>'.str_replace('CHANGE:','',$items[3]).'</td>
						<td colspan="3">('.str_replace('WHERE:','',$items[4]).')</td>';
				}
				$out.='
					</tr>';
				$lineNo++;
			}
			$out.='
				<input type="hidden" name="parms" value="'.$parmList.'">
				<input type="hidden" name="fileLines" value="'.$lineNo.'">';
		}
		$out.='
				<tr>
					<td colspan="5" align="center"><input type="submit" class="button" name="revert" value="Revert"> <input type="submit" class="button" name="checkin" value="CheckIn"></td>
				</tr>
			</table>
		</form>';
		
	}else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=sqlcvs_diff&error=You are not authorised to change the installation.");
			exit(0);
		}

		$host=stripslashes($_POST['host']);
		$username=stripslashes($_POST['username']);
		$password=stripslashes($_POST['password']);
		$parmList=$_POST['parms'];
		
		$fileLines=$_POST['fileLines'];
		$fileArray=file('/tmp/tmp_sqlcvs_file');
		$maskArray=array();
		for($i=0;$i<$fileLines;$i++){
			if(isset($_POST['line_'.$i]) && (int)$_POST['line_'.$i]==1){
				$maskArray[]=$fileArray[$i];
			}
		}
		writeFile('/tmp/tmp_sqlcvs_file',join("\n",$maskArray));
		
		$sqlcvsAction=(isset($_POST['revert']))?'revert':'checkin';
		
		$cmd='/usr/pluto/bin/sqlCVS -H '.$host.' -h localhost -a -n '.$parmList.' -d “'.$username.'” -U "'.$username.'~'.$password.'" -D '.$dbPlutoMainDatabase.' -e -m /tmp/tmp_sqlcvs_file '.$sqlcvsAction;
		unlink('/tmp/tmp_sqlcvs_file');

		
		$out='
		<script>
		
			windowOpen(\'operations/logs/executeLog.php?script=2&command='.urlencode($cmd).'\',\'width=1024,height=768,scrollbars=1,resizable=1\')
			self.location="index.php?section=sqlcvs_diff";		
		</script>';
		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: sqlCVS Diff');
	$output->output();
}

function writeFile($filename,$content)
{
   if (!$handle = fopen($filename, 'w+')) {
         return 1; // Cannot open file ($filename)
    }

    // Write $content to our opened file.
    if (fwrite($handle, $content) === FALSE) {
        return 2;	// Cannot write to file ($filename)
    }	
    
    return 0;
}
?>
