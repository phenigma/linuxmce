<?
function steps($output,$dbADO) {
	/* @var $dbADO ADOConnection */

	$out='';
	$currentSection = @cleanString($_REQUEST['rightSection']);
	$installationID = (int)@$_SESSION['installationID'];
	$currentItem=(isset($_REQUEST['pageSetup']) && (int)$_REQUEST['pageSetup']>0)?(int)$_REQUEST['pageSetup']:$GLOBALS['InstallationPage'];

	if(isset($_REQUEST['senderID']) && (int)$_REQUEST['senderID']>0){
		$senderStep=(int)$_REQUEST['senderID'];
		$querySS='SELECT * FROM SetupStep WHERE FK_Installation=? AND FK_PageSetup=?';
		$resSS=$dbADO->Execute($querySS,array($installationID,$senderStep));
		if($resSS->RecordCount()==0){
			$insertSS='INSERT INTO SetupStep (FK_Installation, FK_PageSetup) VALUES (?,?)';
			$dbADO->Execute($insertSS,array($installationID,$senderStep));
		}
	}
	
	$scriptInHead ='
		<script>
		function setMenuItem(mainUrl,selfPage,sender)
		{
			if(mainUrl!=\'\'){
				if(top.basefrm.document.forms[0] && top.basefrm.document.forms[0].action){
					top.basefrm.document.forms[0].action.value=\'externalSubmit\';
					top.basefrm.document.forms[0].submit();
				}
				top.basefrm.location=mainUrl;
			}
			else
				top.basefrm.location=\'index.php?section=installationSettings\';
				self.location=\'index.php?section=wizard&pageSetup=\'+selfPage+\'&senderID=\'+sender;				
		}
		</script>
	';
	
	$selectMenu = "
		SELECT * FROM PageSetup 
		LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup AND (FK_Installation IS NULL OR FK_Installation=?)
		WHERE FK_PageSetup_Parent = 1 AND showInTopMenu = 1 AND Website=1
		ORDER BY OrderNum ASC
		";
	$resSelectMenu = $dbADO->Execute($selectMenu,$installationID);
	$out.='<table border="0" cellpading="2" cellspacing="0" width="100%">
			<tr>
				<td colspan="3" align="center">
					<a href="index.php"><img src="include/images/logo_pluto.jpg" border="0"></a>
				</td>
			</tr>
			<tr>
				<td colspan="3" align="center">
					<h3>Wizard</h3>
				</td>
			</tr>
	';
	$nextItem=array();
	$nextUrl=array();
	$startPos=0;
	
	$resrootNodes = $dbADO->Execute($selectMenu,$installationID);
	while ($rootNodes = $resrootNodes->FetchRow()) {
		$selectLevel1 = "
			SELECT * FROM PageSetup 
			LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup AND (FK_Installation IS NULL OR FK_Installation=?)
			WHERE FK_PageSetup_Parent = {$rootNodes['PK_PageSetup']} AND showInTopMenu = 1
			ORDER BY OrderNum ASC";
		$resSelectLevel1 = $dbADO->Execute($selectLevel1,$installationID);
		while ($rowSelectLevel1 = $resSelectLevel1->FetchRow()) {
			if($rowSelectLevel1['pageURL']!=''){
				$nextItem[$startPos]=$rowSelectLevel1['PK_PageSetup'];
				$nextUrl[$startPos]=$rowSelectLevel1['pageURL'];
			}
			$startPos=$rowSelectLevel1['PK_PageSetup'];
			
			$selectLevel2 = "
				SELECT * FROM PageSetup
				LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup AND (FK_Installation IS NULL OR FK_Installation=?)
				WHERE FK_PageSetup_Parent = {$rowSelectLevel1['PK_PageSetup']} AND showInTopMenu = 1";
			$resSelectLevel2 = $dbADO->Execute($selectLevel2,$installationID);
			while ($rowSelectLevel2 = $resSelectLevel2->FetchRow()) {
				if($rowSelectLevel2['pageURL']!=''){
					$nextItem[$startPos]=$rowSelectLevel2['PK_PageSetup'];
					$nextUrl[$startPos]=$rowSelectLevel1['pageURL'];
				}
					$startPos=$rowSelectLevel2['PK_PageSetup'];
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
				<td height="22">'.$wizardLink.'</td>
				<td align="right">&nbsp;</td>
			</tr>
			';
		//get childs (ne-recursive method)
		$selectLevel1 = "
			SELECT * FROM PageSetup 
			LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup AND (FK_Installation IS NULL OR FK_Installation=?)
			WHERE FK_PageSetup_Parent = {$rowSelectMenu['PK_PageSetup']} AND showInTopMenu = 1
			ORDER BY OrderNum ASC";

		$resSelectSubMenu1 = $dbADO->Execute($selectLevel1,$installationID);
		while ($rowSelectSubMenu1 = $resSelectSubMenu1->FetchRow()) {

			//get level 2 childs (ne-recursive method)
			$wizardLink=($rowSelectSubMenu1['pageURL']!='')?'<a href="#" onClick="setMenuItem(\''.$rowSelectSubMenu1['pageURL'].'\',\''.@$rowSelectSubMenu1['PK_PageSetup'].'\',\''.@$rowSelectSubMenu1['PK_PageSetup'].'\')">'.$rowSelectSubMenu1['Description'].'</a>':'<b>'.$rowSelectSubMenu1['Description'].'</b>';
			if($rowSelectSubMenu1['FK_Installation']!='')
				$wizardLink.='<img src="include/images/sync.gif" align="middle">';
			$out.='
			<tr>
				<td></td>
				<td bgcolor="'.(($currentItem==$rowSelectSubMenu1['PK_PageSetup'])?'#CCCCCC':'').'">&nbsp;&nbsp;&nbsp;&nbsp;'.$wizardLink.'</td>
				'.(($currentItem==$rowSelectSubMenu1['PK_PageSetup'])?'<td align="right" bgcolor="#CCCCCC"><a href="#" onClick="setMenuItem(\''.@$nextUrl[$rowSelectSubMenu1['PK_PageSetup']].'\',\''.@$nextItem[$rowSelectSubMenu1['PK_PageSetup']].'\',\''.$rowSelectSubMenu1['PK_PageSetup'].'\')">Next</a></td>':'<td>&nbsp;</td>').'
			</tr>';
			$selectLevel2 = "
				SELECT * FROM PageSetup
				LEFT JOIN SetupStep ON FK_PageSetup=PK_PageSetup AND (FK_Installation IS NULL OR FK_Installation=?)
				WHERE FK_PageSetup_Parent = {$rowSelectSubMenu1['PK_PageSetup']} AND showInTopMenu = 1";

			$resSelectSubMenu2 = $dbADO->Execute($selectLevel2,$installationID);

			while ($rowSelectSubMenu2 = $resSelectSubMenu2->FetchRow()) {
				$wizardLink=($rowSelectSubMenu2['pageURL']!='')?'<a href="#" onClick="setMenuItem(\''.$rowSelectSubMenu2['pageURL'].'\',\''.@$rowSelectSubMenu2['PK_PageSetup'].'\',\''.@$rowSelectSubMenu2['PK_PageSetup'].'\')">'.$rowSelectSubMenu2['Description'].'</a>':'<b>'.$rowSelectSubMenu2['Description'].'</b>';
				if($rowSelectSubMenu2['FK_Installation']!='')
					$wizardLink.='<img src="include/images/sync.gif" align="middle">';

			$out.='
			<tr>
				<td></td>
				<td bgcolor="'.(($currentItem==$rowSelectSubMenu2['PK_PageSetup'])?'#CCCCCC':'').'">&nbsp;&nbsp;&nbsp;&nbsp;'.$wizardLink.'</td>
				'.(($currentItem==$rowSelectSubMenu2['PK_PageSetup'])?'<td align="right" bgcolor="#CCCCCC"><a href="#" onClick="setMenuItem(\''.@$nextUrl[$rowSelectSubMenu2['PK_PageSetup']].'\',\''.@$nextItem[$rowSelectSubMenu2['PK_PageSetup']].'\',\''.$rowSelectSubMenu1['PK_PageSetup'].'\')">Next</a></td>':'<td>&nbsp;</td>').'	
			</tr>';
			}
			$resSelectSubMenu2->Close();
		}
		$resSelectSubMenu1->Close();
	}
	$resSelectMenu->Close();
	$out.='
		<tr>
			<td colspan="2" align="center">&nbsp;</td>
		</tr>
		<tr>
			<td colspan="2" align="center"><a href="index.php?section=leftMenu"><B>Back to devices</B></a></td>
		</tr>
	</table>';
	
	$output->setScriptInHead($scriptInHead);
	$output->setScriptInBody('bgColor="#F0F3F8"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}


?>