<?
function leftMenu($output,$dbADO) {
	$out='';
	$currentSection = @cleanString($_REQUEST['rightSection']);
	$installationID = (int)@$_SESSION['installationID'];
	
	$treeType = isset($_REQUEST['treeType'])?cleanString(strip_tags($_REQUEST['treeType'])):'devices';
	$out.= '	
			<table border=0 align="left">				
			<tr>
				<td>
					<img src="include/images/logo_pluto.jpg">
				</td>
			</tr>';
	
	if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {
		$out.='
				<tr>
					<td valign="top">'.($currentSection=='login'?'&raquo;':'').'<a href="index.php?section=login&action=logout" target="basefrm" >Logout</a></td>					
				</tr>
		';
	} else {
		$out.='
				<tr>
					<td valign="top">'.($currentSection=='login'?'&raquo;':'').'<a href="index.php?section=login"  target="basefrm">Login</a></td>
				</tr>
		';
	}
	
	$out.= '
	
				<tr>
					<td valign="top" nowrap>'.($currentSection=='deviceTemplates'?'&raquo;':'').'<a href="index.php?section=deviceTemplates" target="basefrm">Device Templates</a></td>
				</tr>
			
	';	
	
	if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {
		
		$selectedInstallation = isset($_REQUEST['installationNo'])?(int)$_REQUEST['installationNo']:(int)$_SESSION['installationID'];
		//load the selected installation into SESSION (and check if is a valid installation for this user)
		if (in_array($selectedInstallation,$_SESSION['installationIDs'])) {
			$_SESSION['installationID'] = $selectedInstallation;				
		} else {
			$selectedInstallation = $_SESSION['installationID'];
		}
		
		if ($treeType=='scenarios') {
			$jsTree='';
				$query = "SELECT * FROM Array Where CommandGroup = 1 Order BY Description ASC";	
				$res1 = $dbADO->_Execute($query);
					if ($res1) {
						while ($row1 = $res1->FetchRow()) {
							$jsTree.='				    
								auxS'.$row1['PK_Array'].' = insFld(foldersTree, gFld("'.$row1['Description'].'", "index.php?section=myScenarios&action=showArrayCommands&array='.$row1['PK_Array'].'"));
								auxS'.$row1['PK_Array'].'.xID = '.$row1['PK_Array'].';
							';
							
							$queryGetCommands = 'SELECT * FROM CommandGroup WHERE FK_Installation = ? AND FK_Array = ?';
							$resGetCommands = $dbADO->Execute($queryGetCommands,array($installationID,$row1['PK_Array']));
							
							while ($rowGetCommands = $resGetCommands->FetchRow()) {
								$jsTree.='				    
									auxSa'.$rowGetCommands['PK_CommandGroup'].' = insFld(auxS'.$row1['PK_Array'].', gFld("'.$rowGetCommands['Description'].(($rowGetCommands['Hint']!='')?': '.$rowGetCommands['Hint']:'').'", "index.php?section=editCommandGroup&cgID='.$rowGetCommands['PK_CommandGroup'].'"));
									auxSa'.$rowGetCommands['PK_CommandGroup'].'.xID = '.$rowGetCommands['PK_CommandGroup'].';
								';
							}
							
						}
					}			
		}	else {
			$jsTree='';
				$query1 = "select * from Device where FK_Device_ControlledVia IS NULL and FK_Installation = $installationID";
				//$query1 = "select * from Device where FK_Device_ControlledVia IS NULL";
				$res1 = $dbADO->_Execute($query1);
					if ($res1) {
						while ($row1 = $res1->FetchRow()) {
							$jsTree.='				    
								auxS'.$row1['PK_Device'].' = insFld(foldersTree, gFld("'.str_replace('"','\"',$row1['Description']).'", "index.php?section=editDeviceParams&deviceID='.$row1['PK_Device'].'"));
								auxS'.$row1['PK_Device'].'.xID = -'.$row1['PK_Device'].';
							';
							$jsTree.=getDeviceChilds($row1['PK_Device'],$dbADO);
						}
					}			
		}
			
			
			$installationTxt = '';
			
				$installationTxt.='
					<form action="index.php" method="post">
					Change Installation:
					<input type="hidden" name="section" value="leftMenu">
					<input type="hidden" name="rightSection" value="'.$currentSection.'">
					<select name="installationNo" onChange="parent.frames[1].location.href=\'index.php?section=userHome&reloadTree=true\';this.form.submit();">';
			
					foreach ($_SESSION['installationIDs'] as $elem) {
						$installationTxt.="<option ".($selectedInstallation==$elem?" selected ":'')." value='".$elem."'>$elem</option>";
					}

					$installationTxt.='</select> <input type="submit" class="button" name="submitX" value="Go">
				</form>
				';
			
		
			$out.='
			<tr>
				<td><hr /></td>
			</tr>
			<tr>
				<td>
					'.$installationTxt.'
				</td>
			</tr>
			<!--<tr>
				<td>
					<a href="index.php?section=installationSettings" target="basefrm">Settings</a>
				</td>
			</tr>
			<tr>
				<td>
					<a href="index.php?section=rooms" target="basefrm">Rooms</a>
				</td>
			</tr>		-->	
			';		
			
			$scriptInHead = "
			
			<!-- As in a client-side built tree, all the tree infrastructure is put in place
			     within the HEAD block, but the actual tree rendering is trigered within the
			     BODY -->
			
			<!-- Code for browser detection -->
			<script src=\"scripts/treeview/ua.js\"></script>
			
			<!-- Infrastructure code for the tree -->
			<script src=\"scripts/treeview/ftiens4.js\"></script>
			
			<!-- Execution of the code that actually builds the specific tree.
			     The variable foldersTree creates its structure with calls to
				 gFld, insFld, and insDoc -->
			<script>
			USETEXTLINKS = 1
			STARTALLOPEN = 0
			USEFRAMES = 1
			USEICONS = 1
			WRAPTEXT = 1
			PRESERVESTATE = 0
			ICONPATH = 'scripts/treeview/' 
			HIGHLIGHT = 1
			GLOBALTARGET = 'R'
			
			foldersTree = gFld('<b>".($treeType=='scenarios'?'My Scenarios':'My Devices')."</b>', \"index.php?section=".($treeType=='scenarios'?'myScenarios':'myDevices')."&action=showBasicInfo\");
			foldersTree.xID = 1001635872
			$jsTree
			
			".($treeType=='devices'?"
								auxS = insFld(foldersTree, gFld(\"Add top level device\", \"index.php?section=addMyDevice&parentID=0\"));
								auxS.xID = 0;
								auxS.iconSrc = ICONPATH + \"diffFolder.gif\"
			  					auxS.iconSrcClosed = ICONPATH + \"diffFolder.gif\" 
			":'')."
			
			foldersTree.treeID = 't2'
			
			function getObj(obj) {
					if (document.layers) {
						if (typeof obj == 'string') {
							return document.layers[obj];
						} else 	{
							return obj;
						}
					}
					if (document.all) {
						if (typeof obj == 'string') {
							return document.all(obj);
						} else {
							return obj;
						}
					}
					if (getObj) {
						if (typeof obj == 'string') {
							return getObj(obj);
						} else {
							return obj;
						}
					}
					return null;	
				} 
			
				function showHideObject(obj) {
					obj = getObj(obj);					
					if(document.all || getObj) {  			
							if (obj.style.visibility == \"visible\") {
								obj.style.visibility = \"hidden\";
								obj.style.display = \"none\";
							} else {
							    obj.style.visibility = \"visible\";
								obj.style.display = \"block\";
							}
					} else if (document.layers) {
							if (obj.style.visibility == \"visible\") {
								obj.visibility = \"hide\";	
							} else {
							obj.visibility = \"show\";	
							}
					}
				} 
			
			
			
			
			</script>
			";
			
			$out.= '
				<tr valign="top">
					<td valign="top" nowrap>
						<span style="font-color:#FFFFFF">
								<table border=0 valign="top">
									<tr style="display:none">
										<td>
											<font size=-2><a style="font-size:7pt;text-decoration:none;color:#FFFFFF" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font>
										</td>
									</tr>
								</table>
						</span>
								<span>
								<script>
									initializeDocument();						
								</script>
								<noscript>
								A tree for site navigation will open here if you enable JavaScript in your browser.
								</noscript>
					</td>
				</tr>
			<tr>
				<td>
					<br /><a href="index.php?section=leftMenu&treeType='.($treeType=='scenarios'?'devices':'scenarios').'">'.($treeType=='scenarios'?'My Devices':'My Scenarios').'</a>
				</td>
			</tr>
			<!--
			<tr>
				<td>
					<br /><a href="index.php?section=settings" target="basefrm">Use wizard</a>
				</td>
			</tr>
			<tr>
				<td>
					<a href="index.php?section=relatedInstallations" target="basefrm">Related Installations</a>
				</td>
			</tr>
			-->
			';



			$output->setScriptInHead($scriptInHead);

	}



	$out.='</table>';

	$output->setScriptInBody('bgColor="#F0F3F8"');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}


function getDeviceChilds($parentID,$dbADO) {
	$queryGP = "select * from Device where FK_Device_ControlledVia = $parentID";
	$resGP = $dbADO->Execute($queryGP);
	$jsTree='';
	if ($resGP) {
		while ($row=$resGP->FetchRow()) {
				$jsTree.= '
					auxS'.$row['PK_Device'].' = insFld(auxS'.$parentID.', gFld("'.str_replace('"','\"',$row['Description']).'", "index.php?section=editDeviceParams&deviceID='.$row['PK_Device'].'"))
					auxS'.$row['PK_Device'].'.xID = '.$row['PK_Device'].';
				';
				$jsTree.=getDeviceChilds($row['PK_Device'],$dbADO);
		}
	}
	return $jsTree;
}
?>
