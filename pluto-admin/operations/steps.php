<?
function steps($output,$dbADO) {
	$out='';
	$currentSection = @cleanString($_REQUEST['rightSection']);
	$installationID = (int)@$_SESSION['installationID'];
	$currentItem=(isset($_REQUEST['pageSetup']) && (int)$_REQUEST['pageSetup']>0)?(int)$_REQUEST['pageSetup']:$GLOBALS['InstallationPage'];
	$rightFile='index.php?section=installationSettings';

	$selectMenu = "
		SELECT * FROM PageSetup 
		LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup
		WHERE (FK_Installation IS NULL OR FK_Installation=?) AND FK_PageSetup_Parent = 1 AND showInTopMenu = 1 AND Website=1
		";
	$resSelectMenu = $dbADO->Execute($selectMenu,$installationID);
	$out.='<table border="0" cellpading="2" cellspacing="0" width="100%">
			<tr>
				<td colspan="3" align="center">
					<img src="include/images/logo_pluto.jpg">
				</td>
			</tr>
			<tr>
				<td colspan="3" align="center">
					<h3>Wizard</h3>
				</td>
			</tr>
	';
	$nextItem=array();
	$startPos=0;
	
	$resrootNodes = $dbADO->Execute($selectMenu,$installationID);
	while ($rootNodes = $resrootNodes->FetchRow()) {
		$selectLevel1 = "
			SELECT * FROM PageSetup 
			LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup
			WHERE (FK_Installation IS NULL OR FK_Installation=?) AND FK_PageSetup_Parent = {$rootNodes['PK_PageSetup']} AND showInTopMenu = 1";
		$resSelectLevel1 = $dbADO->Execute($selectLevel1,$installationID);
		while ($rowSelectLevel1 = $resSelectLevel1->FetchRow()) {
			if($rowSelectLevel1['pageURL']!='')
				$nextItem[$startPos]=$rowSelectLevel1['PK_PageSetup'];
			$startPos=$rowSelectLevel1['PK_PageSetup'];
			$rightFile=($rowSelectLevel1['PK_PageSetup']==$currentItem)?$rowSelectLevel1['pageURL']:$rightFile;
			
			$selectLevel2 = "
				SELECT * FROM PageSetup
				LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup
				WHERE (FK_Installation IS NULL OR FK_Installation=?) AND FK_PageSetup_Parent = {$rowSelectLevel1['PK_PageSetup']} AND showInTopMenu = 1";
			$resSelectLevel2 = $dbADO->Execute($selectLevel2,$installationID);
			while ($rowSelectLevel2 = $resSelectLevel2->FetchRow()) {
				if($rowSelectLevel2['pageURL']!='')
					$nextItem[$startPos]=$rowSelectLevel2['PK_PageSetup'];
					$startPos=$rowSelectLevel2['PK_PageSetup'];
				$rightFile=($rowSelectLevel2['PK_PageSetup']==$currentItem)?$rowSelectLevel2['pageURL']:$rightFile;
			}
			$resSelectLevel2->Close();
		}
		$resSelectLevel1->Close();
	}
	$resrootNodes->Close();
	
	$resSelectMenu = $dbADO->Execute($selectMenu,$installationID);
	while ($rowSelectMenu = $resSelectMenu->FetchRow()) {
		$wizardLink=($rowSelectMenu['pageURL']!='')?'<a href="'.$rowSelectMenu['pageURL'].'" target="basefrm">'.$rowSelectMenu['Description'].'</a>':'<b>'.$rowSelectMenu['Description'].'</b>';
		if($rowSelectMenu['FK_Installation']!='')
			$wizardLink.='<img src="include/images/sync.gif" align="middle">';
		$out.='
			<tr>
				<td></td>
				<td>'.$wizardLink.'</td>
				<td align="right">&nbsp;</td>
			</tr>
			';
		//get childs (ne-recursive method)
		$selectLevel1 = "
			SELECT * FROM PageSetup 
			LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup
			WHERE (FK_Installation IS NULL OR FK_Installation=?) AND FK_PageSetup_Parent = {$rowSelectMenu['PK_PageSetup']} AND showInTopMenu = 1";

		$resSelectSubMenu1 = $dbADO->Execute($selectLevel1,$installationID);
		while ($rowSelectSubMenu1 = $resSelectSubMenu1->FetchRow()) {

			//get level 2 childs (ne-recursive method)
			$wizardLink=($rowSelectSubMenu1['pageURL']!='')?'<a href="'.$rowSelectSubMenu1['pageURL'].'"  target="basefrm">'.$rowSelectSubMenu1['Description'].'</a>':'<b>'.$rowSelectSubMenu1['Description'].'</b>';
			if($rowSelectSubMenu1['FK_Installation']!='')
				$wizardLink.='<img src="include/images/sync.gif" align="middle">';
			$out.='
			<tr>
				<td></td>
				<td bgcolor="'.(($currentItem==$rowSelectSubMenu1['PK_PageSetup'])?'#CCCCCC':'').'">&nbsp;&nbsp;&nbsp;&nbsp;'.$wizardLink.'</td>
				<td align="right" bgcolor="'.(($currentItem==$rowSelectSubMenu1['PK_PageSetup'])?'#CCCCCC':'').'"><a href="index.php?section=wizard&pageSetup='.@$nextItem[$rowSelectSubMenu1['PK_PageSetup']].'" target="_self">Next'.'</a></td>
			</tr>';
			$selectLevel2 = "
				SELECT * FROM PageSetup
				LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup
				WHERE (FK_Installation IS NULL OR FK_Installation=?) AND FK_PageSetup_Parent = {$rowSelectSubMenu1['PK_PageSetup']} AND showInTopMenu = 1";

			$resSelectSubMenu2 = $dbADO->Execute($selectLevel2,$installationID);

			while ($rowSelectSubMenu2 = $resSelectSubMenu2->FetchRow()) {
				$wizardLink=($rowSelectSubMenu2['pageURL']!='')?'<a href="'.$rowSelectSubMenu2['pageURL'].'"  target="basefrm">'.$rowSelectSubMenu2['Description'].'</a>':'<b>'.$rowSelectSubMenu2['Description'].'</b>';
				if($rowSelectSubMenu2['FK_Installation']!='')
					$wizardLink.='<img src="include/images/sync.gif" align="middle">';

			$out.='
			<tr>
				<td></td>
				<td bgcolor="'.(($currentItem==$rowSelectSubMenu2['PK_PageSetup'])?'#CCCCCC':'').'">&nbsp;&nbsp;&nbsp;&nbsp;'.$wizardLink.'</td>
				<td align="right" bgcolor="'.(($currentItem==$rowSelectSubMenu2['PK_PageSetup'])?'#CCCCCC':'').'"><a href="index.php?section=wizard&pageSetup='.@$nextItem[$rowSelectSubMenu2['PK_PageSetup']].'" target="_self">Next</a></td>
			</tr>';
			}
			$resSelectSubMenu2->Close();
		}
		$resSelectSubMenu1->Close();
	}
	$resSelectMenu->Close();
	$out.='</table>';
	
	$output->setScriptInBody('bgColor="#F0F3F8" onLoad="top.basefrm.location=\''.$rightFile.'\'"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}


?>