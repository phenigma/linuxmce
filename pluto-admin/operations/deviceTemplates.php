<?
function deviceTemplates($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$userID= (int)@$_SESSION['userID'];
$out='';
$dbADO->debug=false;
$selectedManufacturer = (int)@$_REQUEST['manufacturers'];
$selectedDeviceCateg= (int)@$_REQUEST['deviceCategSelected'];
$selectedDevice= (int)@$_REQUEST['deviceSelected'];
$selectedModel= (int)@$_REQUEST['model'];
$justAddedNode = (int)@$_GET['justAddedNode'];


$actionX = (isset($_REQUEST['actionX']) && cleanString($_REQUEST['actionX'])!='null')?cleanString($_REQUEST['actionX']):'form';

$selectManufacturersTxt = '';
$queryManufacturers = "SELECT * FROM Manufacturer ORDER BY Description";
$rs = $dbADO->_Execute($queryManufacturers);
	while ($row = $rs->FetchRow()) {
		$selectManufacturersTxt.="<option ".($selectedManufacturer==$row['PK_Manufacturer']?" selected ":"")." value='{$row['PK_Manufacturer']}'>{$row['Description']}</option>";
	}
$rs->Close();

if ($actionX=='form') {


	$jsTree = '';
	
	$selectDeviceCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent IS NULL ORDER BY Description";
	$rs = $dbADO->_Execute($selectDeviceCategories);
		while ($row = $rs->FetchRow()) {		
			$jsTree.='
			auxS'.$row['PK_DeviceCategory'].' = insFld(foldersTree, gFld("'.$row['Description'].' #'.$row['PK_DeviceCategory'].'", "javascript:parent.frames[1].document.forms[0].deviceCategSelected.value=0;parent.frames[1].document.forms[0].deviceSelected.value='.$row['PK_DeviceCategory'].';parent.frames[1].document.forms[0].actionX.value=null;parent.frames[1].document.forms[0].submit();"));
			auxS'.$row['PK_DeviceCategory'].'.xID = '.$row['PK_DeviceCategory'].';
			';
			$jsTree.=getChilds($row['PK_DeviceCategory'],$dbADO);
		}
	$rs->Close();
	
	
	$selectModels = '';
	$modelsJsArray = '
			modelsArray = new Array();
	';
	$where = " and 1=1 ";
	$manufOrDevice = 0;
	if ($selectedManufacturer!=0) {	
		$where.=" and FK_Manufacturer = '$selectedManufacturer'";
		$manufOrDevice = 1;
	}
	
	if ($selectedDevice!=0) {
		if ($selectedDeviceCateg==0) {
			$GLOBALS['childsDeviceCategoryArray'] = array();
			
			getDeviceCategoryChildsArray($selectedDevice,$dbADO);
			$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
			$GLOBALS['childsDeviceCategoryArray'][]=$selectedDevice;
			
			$childArray = join(",",$GLOBALS['childsDeviceCategoryArray']);		
			
			$where.=" and DeviceCategory.PK_DeviceCategory in ($childArray)";
		} else {		
			$where .= " and DeviceCategory.PK_DeviceCategory = '$selectedDevice'";	
		}
		$manufOrDevice = 1;
	}
	
	if ($manufOrDevice)	{
	
		$queryModels = "SELECT DeviceTemplate.*,DeviceCategory.Description as deviceDescription,
								Manufacturer.Description as manufacturerDescription 
						FROM DeviceTemplate INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
							  				  INNER JOIN Manufacturer ON PK_Manufacturer=FK_Manufacturer
						WHERE 1=1 $where ORDER BY Description";	
		
		
		$rs = $dbADO->_Execute($queryModels);
		
		$arJsPos=0;
			while ($row = $rs->FetchRow()) {
				$selectModels.="<option ".($selectedModel==$row['PK_DeviceTemplate']?" selected ":"")." value='{$row['PK_DeviceTemplate']}'>{$row['Description']}</option>";
				
				if ($row['PK_DeviceTemplate']>0) {
				$modelsJsArray.='
				
				model'.$row['PK_DeviceTemplate'].' = new Array();
					model'.$row['PK_DeviceTemplate'].'.manufacturerName = \''.$row['manufacturerDescription'].'\';
					model'.$row['PK_DeviceTemplate'].'.deviceDescription = \''.$row['deviceDescription'].'\';
				    model'.$row['PK_DeviceTemplate'].'.userID = \''.@$_SESSION['userID'].'\';
				
				modelsArray['.$row['PK_DeviceTemplate'].'] = model'.$row['PK_DeviceTemplate'].';			
				
				';
				}
				$arJsPos++;
			}
		$rs->Close();
	}
	
		
	
	$scriptInHead = "
	
	<script>
	function windowOpen(locationA,attributes) {
		window.open(locationA,'',attributes);
	}
	</script>
	
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
	USEFRAMES = 0
	USEICONS = 0
	WRAPTEXT = 1
	PRESERVESTATE = 1
	ICONPATH = 'scripts/treeview/' 
	HIGHLIGHT = 1
	
	
	foldersTree = gFld('<b>Device Category</b>', \"javascript:parent.frames[1].document.forms[0].deviceSelected.value=0;parent.frames[1].document.forms[0].deviceCategSelected.value=0;parent.frames[1].document.forms[0].actionX.value=null;parent.frames[1].document.forms[0].submit();\");
	foldersTree.xID = 1001635872;
	$jsTree
	
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
	$modelsJsArray
	
	
	</script>
	";
	$output->setScriptInHead($scriptInHead);
	$out.='
	<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']):'').'</div>	
	<form action="'.$_SERVER['PHP_SELF'].'" method="POST" name="deviceTemplates">
	<input type="hidden" name="section" value="deviceTemplates">
	<input type="hidden" name="actionX" value="add">
	<input type="hidden" name="deviceCategSelected" value="'.$selectedDeviceCateg.'">
	<input type="hidden" name="deviceSelected" value="'.$selectedDevice.'">
	<input type="hidden" name="modelSelected" value="'.$selectedModel.'">

	
	<table cellpadding="5" cellspacing="0" border="0" align="center">
			<input type="hidden" name="selectedField" value="" />
				<tr>
					<th width="25%">Manufacturers</th>
					<th width="25%">Device Category</th>
					<th width="25%">
					Models
					
					</th>				
				</tr>
				<tr valign="top">
					<td width="25%" align="center"  valign="top">
						<select name="manufacturers" id="select_Manufacturer" onchange="this.form.selectedField.value=\'manufacturers\';this.form.actionX.value=\'form\';this.form.submit();" size="10" style="z-index:1;">
							<option value="" selected="selected">All</option>
							'.$selectManufacturersTxt.'						
						</select><br /><br />
						<input type="text" name="Manufacturer_Description" size="15" />
						<input type="submit" name="addManufacturer" value="Add" />
						<br /><br /><br /><br />					
						<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=manageCategories&action=commands&from=deviceTemplates\',\'width=600,height=800,toolbars=true\');">Manage Commands Categories</a><br />
						<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=manageCategories&action=events&from=deviceTemplates\',\'width=600,height=800,toolbars=true\');">Manage Events Categories</a><br />
					</td>
					<td width="25%" align="center" valign="top">
						<span style="font-color:#FFFFFF"><table border=0><tr><td><font size=-2><a style="font-size:7pt;text-decoration:none;color:#FFFFFF" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font></td></tr></table></span>
						<span>
						<script>
							initializeDocument();						
						</script>
						<noscript>
						A tree for site navigation will open here if you enable JavaScript in your browser.
						</noscript>
						</span>
						<br /><br />
						<input type="text" name="DeviceCategory_Description" size="15" />
						<input type="submit" name="addDeviceCategory" value="Add '.($selectedDevice==0?' Top Level Child':' Child').'" />
						'.($selectedDevice!=0?'<input type="button" name="editDeviceCategory" value="Edit" onClick="javascript: windowOpen(\'index.php?section=editDeviceCategory&from=deviceTemplates&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'\',\'status=0,resizable=1,width=600,height=250,toolbars=true\');" />':'<input type="submit" name="editDeviceCategory" value="Edit" disabled="disabled" />').'';
						getDeviceCategoryChildsNo($selectedDevice,$dbADO);					
						$childsToDelete = $GLOBALS['childsDeviceCategoryNo'];
						$out.='
						&nbsp;&nbsp;'.($selectedDevice!=0?'<input type="button" name="deteleDeviceCategory" value="Delete" onClick="javascript: if (confirm(\'Are you sure you want to delete this device category?'.($childsToDelete==1?'There is 1 child to delete!':($childsToDelete>0?'There are '.$childsToDelete.' childs to delete!':'')).'\')) windowOpen(\'index.php?section=deleteDeviceCategory&from=deviceTemplates&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'\',\'status=0,resizable=1,width=100,height=100,toolbars=0\');" />':'<input type="submit" name="deleteDeviceCategory" value="Delete" disabled="disabled" />').'
					</td>
					<td width="25%" align="center"  valign="top">
						<script>
							function checkEdit(objF) {							
									objF.section.value=\'editMasterDevice\';
									objF.submit();
							}
	
							function updateModelDetail(objF) {
								if (typeof modelsArray[objF.model[objF.model.selectedIndex].value] != "undefined") {
									getObj(\'modelManuf\').innerHTML = \'<br />\'+modelsArray[objF.model[objF.model.selectedIndex].value].manufacturerName;
									getObj(\'modelDeviceDescription\').innerHTML = \'<br />\'+modelsArray[objF.model[objF.model.selectedIndex].value].deviceDescription;
								} else {
									getObj(\'modelManuf\').innerHTML = \'\';
									getObj(\'modelDeviceDescription\').innerHTML = \'\';
								} 					
							}
							
						</script>
						<select name="model" id="model" 
								onChange="updateModelDetail(this.form);"
								 size="10">
								<option value="" selected="selected">All</option>
								'.$selectModels.'	
						</select>
						<input type="button" name="edit_DeviceTemplate" value="Edit" onClick="javascript:checkEdit(this.form);" />
						<hr />
					<b><span id="modelManuf"></span><span id="modelDeviceDescription"></span></b><br />
						<b>New:</b>
						';
						
						if ($selectedManufacturer!=0 && $selectedDevice!=0) {
							$disabledAdd=' ';
						} else {
							$disabledAdd=' disabled="disabled" ';
						}
				
						$disabled_str = "disabled";
						
			$out.='		<input type="text" name="DeviceTemplate_Description" size="15"'.$disabledAdd.' />
						<input type="submit" name="addDeviceTemplate" value="Add"'. $disabledAdd .' />
					</td>				
				</tr>
			</table>
						
	</form>
	';
} else {
	
	 $addDeviceCategory = @cleanString($_POST['addDeviceCategory']);
	 $deviceCategoryDesc = cleanString($_POST['DeviceCategory_Description']);
	 
	 $addDeviceTemplate = @cleanString($_POST['addDeviceTemplate']);
	 $DeviceTemplate_Desc = @cleanString($_POST['DeviceTemplate_Description']);

	 $addManufacturer = @cleanString($_POST['addManufacturer']);
	 $Manufacturer_Description = @cleanString($_POST['Manufacturer_Description']);	 
	
	 $deviceCategSelected = (int)$_POST['deviceCategSelected'];
	 $deviceSelected = (int)$_POST['deviceSelected'];
	 $manufacturerSelected = (int)$_POST['manufacturers'];

	 // process form only if user is logged
	 if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']==true) {

		 // add new category
		 $justAddedNode = 0;
		 if (strstr($addDeviceCategory,'Add')) {
		 	if ($deviceCategoryDesc!='') {
		 		if ($deviceSelected==0) {
		 			$queryInsertCategoryDesc = "insert into DeviceCategory(FK_DeviceCategory_Parent,Description,IsAV)
		 				values(NULL,?,0)";
		 			$dbADO->Execute($queryInsertCategoryDesc,array($deviceCategoryDesc));	 			
		 		} else {
		 			$queryInsertCategoryDesc = "insert into DeviceCategory(FK_DeviceCategory_Parent,Description,IsAV)
		 				values(?,?,0)";
		 			$dbADO->Execute($queryInsertCategoryDesc,array($deviceSelected,$deviceCategoryDesc));
		 			$justAddedNode = $deviceSelected;
		 		}	 			 		
			}
		 }	
		 	 
		 //add new master device list
		 if (strstr($addDeviceTemplate,'Add')) {
		 	
		 	if ($DeviceTemplate_Desc!='') {
			 		
		 		if ($deviceSelected!=0 && $manufacturerSelected!=0) {	 			
		 			$queryInsertMasterDevice = 'INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer) values(?,?,?)';
		 			$res = $dbADO->Execute($queryInsertMasterDevice,array($DeviceTemplate_Desc,$deviceSelected,$manufacturerSelected));	 			
		 		}	 			 		
		 	}
		 }
		 
		 // add new manufacturer
		 if (strstr($addManufacturer,'Add')){
		 	if ($Manufacturer_Description!='') {
	 			$queryInsertManufacturer = 'INSERT INTO Manufacturer (Description) values(?)';
	 			$res = $dbADO->Execute($queryInsertManufacturer,array($Manufacturer_Description));	 			
	 		}	 			 		
		 }
		 header("Location: index.php?section=deviceTemplates&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&justAddedNode=$justAddedNode");
	}else{
		header("Location: index.php?section=deviceTemplates&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&error=Please login if you want to change device templates.");
	}
}

$output->setNavigationMenu(array("Device Templates"=>'index.php?section=deviceTemplates'));

//$output->setScriptInBody("onLoad='javascript:alert();'");
$output->setScriptCalendar('null');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}
function getChilds($parentID,$dbADO) {
	
	$selectDevicesFromCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent = {$parentID} ORDER BY Description";
		$rs2 = $dbADO->_Execute($selectDevicesFromCategories);
		$jsTree='';
			while ($row2 = $rs2->FetchRow()) {		
				$jsTree.= '
					auxS'.$row2['PK_DeviceCategory'].' = insFld(auxS'.$parentID.', gFld("'.$row2['Description'].' #'.$row2['PK_DeviceCategory'].'", "javascript:parent.frames[1].document.forms[0].deviceCategSelected.value='.$parentID.';parent.frames[1].document.forms[0].deviceSelected.value='.$row2['PK_DeviceCategory'].';parent.frames[1].document.forms[0].actionX.value=null;parent.frames[1].document.forms[0].submit();"))
					auxS'.$row2['PK_DeviceCategory'].'.xID = '.$row2['PK_DeviceCategory'].';
				';
				$jsTree.=getChilds($row2['PK_DeviceCategory'],$dbADO);
			}

	return $jsTree;
}
?>