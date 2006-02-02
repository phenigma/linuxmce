<?
function translationItems($output,$dbADO) {
	global $dbPlutoMainServer,$dbPlutoMainUser,$dbPlutoMainPass;
	
	/* @var $dbADO ADOConnection */
	// todo: move records in pluto_main database
	$trADO = &ADONewConnection('mysql');
	$trADO->NConnect($dbPlutoMainServer,urlencode($dbPlutoMainUser),urlencode($dbPlutoMainPass),'translation'); 
	
	$selectedLang=(isset($_REQUEST['language']))?$_REQUEST['language']:0;
	$show_missing=(isset($_REQUEST['show_missing']))?(int)$_REQUEST['show_missing']:0;
	
	$languagesArray=getAssocArray('Language','PK_Language','Description',$dbADO,'WHERE PK_Language!=1');

	if($selectedLang!=0){
		$res=$trADO->Execute('
			SELECT Translation.*,FK_Translation, Translation_Language.Value AS Translated
			FROM Translation
			LEFT JOIN Translation_Language ON FK_Translation=PK_Translation AND FK_Language=?
			ORDER BY Filename ASC,Variable ASC',$selectedLang);
		$file='';
		$table='<table>';
		while($row=$res->FetchRow()){
			if($file!=$row['Filename']){
				$file=$row['Filename'];
				$table.='
					<tr>	
						<td bgcolor="lightblue" colspan="2"><B>'.$row['Filename'].'</B></td>
					</tr>';
			}
			if((int)$row['FK_Translation']==0 || ((int)$row['FK_Translation']!=0 && $show_missing==0)){
				$check_mark=($row['Translated']!='')?'<img src="include/images/sync.gif">':'<span class="err">N.A.</span>';
				$table.='
					<tr>	
						<td><a href="index.php?section=editTranslationItem&trID='.$row['PK_Translation'].'&language='.$selectedLang.'">'.$row['Variable'].'</a></td>
						<td>'.$check_mark.'</td>
					</tr>';
			}
			
		}
		$table.='</table>';
	}
		
	$out='<h3>Translation items</h3>
		<form action="index.php" method="post" name="translationItems">
		<input type="hidden" name="section" value="translationItems">
		<input type="hidden" name="action" value="form">
		<table>
			<tr>
				<td>Choose language:</td>
				<td>'.pulldownFromArray($languagesArray,'language',$selectedLang,'onchange="document.translationItems.submit();"').'</td>
			</tr>
			<tr>
				<td>Show only missing files</td>
				<td><input type="checkbox" name="show_missing" value="1" '.(($show_missing==1)?'checked':'').' onclick="document.translationItems.submit();"></td>
			</tr>
			<tr>
				<td colspan="2"><a href="index.php?section=addTranslationItem">Add item</a></td>
			</tr>	
		</table>
		'.@$table.'
		</form>	
	';
	
	
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Translation items');
	$output->output();  
}
?>