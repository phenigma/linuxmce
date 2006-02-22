<?
function sqlcvs_diff($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/sqlcvs.lang.php');
	
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
		function selAllCheckboxes(group,val)
		{
		   eval("sqlcvs_diff.table_"+group+".checked="+val);
		   for (i = 0; i < sqlcvs_diff.elements.length; i++)
		   {
			tmpName=sqlcvs_diff.elements[i].name;
		     if (sqlcvs_diff.elements[i].type == "checkbox" && tmpName.indexOf(group+"_")!=-1)
		     {
		         sqlcvs_diff.elements[i].checked = val;
		     }
		   }
		}
		
		function groupCheck(group,tablename){
			eval("val=(document.sqlcvs_diff.table_"+group+".checked)?true:false");
			eval("tval=(document.sqlcvs_diff."+group+"_"+tablename+".checked)?true:false");
			if(tval==true && val==false){
				eval("sqlcvs_diff.table_"+group+".checked=true");
			}
		}
		</script>
				
		<div class="err" align="center">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="sqlcvs_diff">
		<input type="hidden" name="section" value="sqlcvs_diff">
		<input type="hidden" name="action" value="choose">	
		
	<div align="center"><h3>'.$TEXT_SQLCVS_DIFF_CONST.'</h3></div>
	<table width="400" cellpadding="3" cellspacing="0">
		<tr>
			<td colspan="3"><B>'.$TEXT_SQLCVS_HOST_CONST.':</B></td>
			<td><input type="text" name="host" value=""></td>
		</tr>
		<tr>
			<td colspan="3"><B>'.$TEXT_PORT_CONST.':</B></td>
			<td><input type="text" name="port" value="3999"></td>
		</tr>		
		<tr>
			<td colspan="3"><B>'.$TEXT_USERNAME_CONST.':</B></td>
			<td><input type="text" name="username" value=""></td>
		</tr>
		<tr>
			<td colspan="3"><B>'.$TEXT_PASSWORD_CONST.':</B></td>
			<td><input type="text" name="password" value=""></td>
		</tr>
		<tr>
			<td colspan="3">&nbsp;</td>
			<td><input type="submit" class="button" name="submit" value="'.$TEXT_NEXT_CONST.'"></td>
		</tr>		
		';
	

	for($i=0;$i<count($tablesArray);$i++){
		$table=$tablesArray[$i];
		if(preg_match('/psc_(.*)_tables/',$table)){
			$cleanTable=str_replace(array('psc_','_tables'),array('',''),$table);
			$out.='
			<tr bgcolor="#F0F3F8">
				<td>&nbsp;</td>
				<td width="20"><input type="checkbox" name="table_'.$cleanTable.'" value="1"></td>
				<td colspan="2"><B>'.$cleanTable.'</B></td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td colspan="2">&nbsp;</td>
				<td colspan="2"><a href="javascript:selAllCheckboxes(\''.$cleanTable.'\',true);">[ Check all ]</a> <a href="javascript:selAllCheckboxes(\''.$cleanTable.'\',false);">[ Uncheck all ]</a></td>
			</tr>			';
			$fieldsArray=getAssocArray($table,'PK_'.$table,'Tablename',$dbADO,'','ORDER BY Tablename ASC');
			foreach ($fieldsArray AS $key=>$value){
				$out.='
			<tr>
				<td>&nbsp;</td>
				<td width="20">&nbsp;</td>
				<td width="20"><input type="checkbox" name="'.$cleanTable.'_'.$value.'" value="1" onClick="groupCheck(\''.$cleanTable.'\',\''.$value.'\')"></td>
				<td>'.$value.'</td>
			</tr>';
			}
		}
	}	
	$out.='	
		<tr>
			<td colspan="3">&nbsp;</td>
			<td><input type="submit" class="button" name="submit" value="'.$TEXT_NEXT_CONST.'"></td>
		</tr>		
	</table>
	</form>
	';
	} elseif($action=='choose'){
		$host=stripslashes($_POST['host']);
		$port=(int)$_POST['port'];
		$username=stripslashes($_POST['username']);
		$password=stripslashes($_POST['password']);
		$rParmArray=array();
		$tParmArray=array();
		
		if($host=='' || $port==''){
			header("Location: index.php?section=sqlcvs_diff&error=$TEXT_ERROR_HOST_OR_PORT_NOT_SPECIFIED_CONST");
			exit();			
		}		
		
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
		
		$parmList='-r '.join(',',$rParmArray).' -t ';
		foreach ($rParmArray AS $rep){
			if(!isset($tParmArray[$rep])){
				header("Location: index.php?section=sqlcvs_diff&error=$TEXT_ERROR_NO_TABLE_SELECTED_CONST");
				exit();
			}
			$parmList.=(substr($parmList,-1)==' ')?',':'';			
			$parmList.=join(',',$tParmArray[$rep]);
		}
		
		$cmd='sudo -u root /usr/pluto/bin/sqlCVS -R '.$port.' -H '.$host.' -h localhost -a -n '.$parmList.' -d "'.$username.'" -U "'.$username.'~'.$password.'" -D '.$dbPlutoMainDatabase.' -e -f /tmp/tmp_sqlcvs_file diff';
		
		exec($cmd,$retArray,$retVal);
		
		$out.='
		<script>
		function selAllCheckboxes(noItems,val)
		{
		   for (i = 0; i < noItems; i++)
		   {
				eval("sqlcvs_diff.line_"+i+".checked="+val);
		   }
		}
		</script>
	<form action="index.php" method="POST" name="sqlcvs_diff">
		<input type="hidden" name="section" value="sqlcvs_diff">
		<input type="hidden" name="action" value="finish">	
		
		<table>
		<tr>
			<td><B>'.$TEXT_SQLCVS_HOST_CONST.':</B></td>
			<td><input type="text" name="host" value="'.$host.'"></td>
		</tr>
		<tr>
			<td><B>'.$TEXT_PORT_CONST.':</B></td>
			<td><input type="text" name="port" value="'.$port.'"></td>
		</tr>		
		<tr>
			<td><B>'.$TEXT_USERNAME_CONST.':</B></td>
			<td><input type="text" name="username" value="'.$username.'"></td>
		</tr>
		<tr>
			<td><B>'.$TEXT_PASSWORD_CONST.':</B></td>
			<td><input type="text" name="password" value="'.$password.'"></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td><input type="submit" class="button" name="submit" value="'.$TEXT_NEXT_CONST.'"></td>
		</tr>
		</table>
		<table width="700" cellpadding="3" cellspacing="0" border="0">';
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
						<td colspan="5"><B>'.$TEXT_REPOSITORY_CONST.': '.$lineRep.'</B></td>
					</tr>';
				}
				$lineTable=str_replace('TABLE:','',$items[1]);
				if($lineTable!=$table){
					$table=$lineTable;
					$out.='
					<tr>
						<td colspan="5"><B>'.$TEXT_TABLE_CONST.': '.$lineTable.'</B></td>
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
						<td><input type="checkbox" name="line_'.$lineNo.'" value="1" checked></td>';
				if(str_replace('CHANGE:','',$items[3])!='DEL'){
					$out.='
						<td><a href="javascript:windowOpen(\'showRecord.php?table='.$lineTable.'&where='.urlencode(str_replace('WHERE:','',$items[4])).'\',\'width=800,height=400,scrollbars=1,resizable=1\');">'.str_replace('CHANGE:','',$items[3]).'</a></td>
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
		if(@count($resultArray)>0){
			$out.='
				<tr>
					<td colspan="5" bgcolor="#F0F3F8"><a href="javascript:selAllCheckboxes(\''.$lineNo.'\',true);">[ Check all ]</a> <a href="javascript:selAllCheckboxes(\''.$lineNo.'\',false);">[ Uncheck all ]</a></td>
				</tr>			
				<tr>
					<td colspan="5" align="center"><input type="submit" class="button" name="revert" value="'.$TEXT_REVERT_CONST.'"> <input type="submit" class="button" name="checkin" value="'.$TEXT_CHECKIN_CONST.'"></td>
				</tr>';
		}else{
			$out.='
				<tr>
					<td colspan="5" align="center">'.$TEXT_NO_CHANGES_CONST.' <a href="index.php?section=sqlcvs_diff">'.$TEXT_TRY_AGAIN_CONST.'</a></td>
				</tr>';
		}
			$out.='
			</table>
		</form>';
		
	}else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=sqlcvs_diff&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		$host=stripslashes($_POST['host']);
		$username=stripslashes($_POST['username']);
		$password=stripslashes($_POST['password']);
		$parmList=$_POST['parms'];
		$port=(int)$_POST['port'];
		
		if($host=='' || $port==''){
			header("Location: index.php?section=sqlcvs_diff&error=$TEXT_ERROR_HOST_OR_PORT_NOT_SPECIFIED_CONST");
			exit();			
		}		
		
		$fileLines=$_POST['fileLines'];
		$fileArray=file('/tmp/tmp_sqlcvs_file');
		$maskArray=array();
		for($i=0;$i<$fileLines;$i++){
			if(isset($_POST['line_'.$i]) && (int)$_POST['line_'.$i]==1){
				$maskArray[]=$fileArray[$i];
			}
		}
		exec('sudo -u root chmod 777 /tmp/tmp_sqlcvs_file');
		writeFile('/tmp/tmp_sqlcvs_file',join("\n",$maskArray)).' '.join("\n",$maskArray);
		
		$sqlcvsAction=(isset($_POST['revert']))?'revert':'checkin';
		
		$cmd='sudo -u root /usr/pluto/bin/sqlCVS -R '.$port.' -H '.$host.' -h localhost -a -n '.$parmList.' -d "'.$username.'" -U "'.$username.'~'.$password.'" -D '.$dbPlutoMainDatabase.' -e -m /tmp/tmp_sqlcvs_file '.$sqlcvsAction;
//		unlink('/tmp/tmp_sqlcvs_file');

		
		$out='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}

		
			windowOpen(\'operations/logs/executeLog.php?script=2&command='.urlencode($cmd).'\',\'width=1024,height=768,scrollbars=1,resizable=1\')
			self.location="index.php?section=sqlcvs_diff";		
		</script>';
		
	}

	$output->setNavigationMenu(array($TEXT_SQLCVS_DIFF_CONST=>'index.php?section=sqlcvs_diff'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SQLCVS_DIFF_CONST);
	$output->output();
}

?>
