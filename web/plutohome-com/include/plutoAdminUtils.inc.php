<?

function pickDeviceTemplate($categoryID, $boolManufacturer,$boolCategory,$boolDeviceTemplate,$returnValue,$defaultAll,$section,$firstColText,$dbADO,$useframes=0,$genericSerialDevicesOnly=0)
{
	$redirectUrl='index.php?section='.$section;

	global $dbPlutoMainDatabase;
	$out='
	<script>
	function windowOpen(locationA,attributes) {
		window.open(locationA,\'\',attributes);
	}
	</script>';
	
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
		if(is_null($categoryID))
			$selectDeviceCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent IS NULL ORDER BY Description";
		else 
			$selectDeviceCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent ='$categoryID' ORDER BY Description";
		$rs = $dbADO->_Execute($selectDeviceCategories);
			while ($row = $rs->FetchRow()) {		
				$jsTree.='
				auxS'.$row['PK_DeviceCategory'].' = insFld(foldersTree, gFld("'.$row['Description'].' #'.$row['PK_DeviceCategory'].'", "javascript:document.forms[1].deviceCategSelected.value=0;document.forms[1].deviceSelected.value='.$row['PK_DeviceCategory'].';document.forms[1].actionX.value=null;document.forms[1].submit();"));
				auxS'.$row['PK_DeviceCategory'].'.xID = '.$row['PK_DeviceCategory'].';
				';
				$jsTree.=getChilds($row['PK_DeviceCategory'],$dbADO);
			}
		$rs->Close();
		
		
		$selectModels = '';
		$modelsJsArray = '
				modelsArray = new Array();
		';
		$irgJsArray='
				irgsArray = new Array();
		';
		$where = " AND 1=1 ";
		$manufOrDevice = 0;
		if ($selectedManufacturer!=0) {	
			$where.=" AND FK_Manufacturer = '$selectedManufacturer'";
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
		}elseif(!is_null($categoryID)){
			$GLOBALS['childsDeviceCategoryArray'] = array();
				
			getDeviceCategoryChildsArray($categoryID,$dbADO);
			$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
			$GLOBALS['childsDeviceCategoryArray'][]=$categoryID;
			
			$childArray = join(",",$GLOBALS['childsDeviceCategoryArray']);		
			$where.=" and DeviceCategory.PK_DeviceCategory in ($childArray)";
		}

		$where.=(($genericSerialDevicesOnly==1)?' AND CommandLine=\''.$GLOBALS['GenericSerialDeviceCommandLine'].'\'':'');
		
		if ($manufOrDevice || $defaultAll==1)	{
		
			$queryModels = "SELECT DeviceTemplate.*,DeviceCategory.Description as deviceDescription,
									Manufacturer.Description as manufacturerDescription 
							FROM DeviceTemplate INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
								  				  INNER JOIN Manufacturer ON PK_Manufacturer=FK_Manufacturer
							WHERE 1=1 $where ORDER BY Description";	
			$rs = $dbADO->_Execute($queryModels);
			
			$arJsPos=0;
				$avDT=getAssocArray('DeviceTemplate_AV','FK_DeviceTemplate','UsesIR',$dbADO);
				while ($row = $rs->FetchRow()) {
					$irHighlight=(in_array($row['PK_DeviceTemplate'],array_keys($avDT)) && $avDT[$row['PK_DeviceTemplate']]==1)?'style="background-color:#FFDFDF;"':'';
					
					$selectModels.="<option ".($selectedModel==$row['PK_DeviceTemplate']?" selected ":"")." value='{$row['PK_DeviceTemplate']}' ".(($row['IsPlugAndPlay']==1)?'style="background-color:lightgreen;"':'')." $irHighlight>{$row['Description']}</option>";
					
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
		
		$selectIrgc='';
		$resIrg=$dbADO->Execute("
			SELECT InfraredGroup.Description,PK_InfraredGroup,Manufacturer.Description AS manufacturerDescription, DeviceCategory.Description as deviceDescription
			FROM InfraredGroup 
				INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
				INNER JOIN Manufacturer ON PK_Manufacturer=FK_Manufacturer
			WHERE 1=1 $where ORDER BY InfraredGroup.Description");
		while($row=$resIrg->FetchRow()){
			$selectIrgc.='<option value="'.$row['PK_InfraredGroup'].'">'.$row['Description'].'</option>';
			
			$irgJsArray.='
					
				irg'.$row['PK_InfraredGroup'].' = new Array();
				irg'.$row['PK_InfraredGroup'].'.manufacturerName = \''.addslashes($row['manufacturerDescription']).'\';
				irg'.$row['PK_InfraredGroup'].'.deviceDescription = \''.addslashes($row['deviceDescription']).'\';
					
				irgsArray['.$row['PK_InfraredGroup'].'] = irg'.$row['PK_InfraredGroup'].';
			';			
			
		}
			
		
		$scriptInHead = "
		<style>
		a{
			text-decoration: none;
			color: #0166FF;
			font-size: 10px;
		}
		
		a:hover {
			text-decoration: none;
			color: #0F46AD;
			font-size: 10px;
		}
		</style>
		
		<!-- As in a client-side built tree, all the tree infrastructure is put in place
		     within the HEAD block, but the actual tree rendering is trigered within the
		     BODY -->
		
		<!-- Code for browser detection -->
		<script src=\"javascripts/treeview/treeview/ua.js\"></script>
		
		<!-- Infrastructure code for the tree -->
		<script src=\"javascripts/treeview/ftiens4.js\"></script>
		
		<!-- Execution of the code that actually builds the specific tree.
		     The variable foldersTree creates its structure with calls to
			 gFld, insFld, and insDoc -->
		<script>
		cssClass='normaltext'
		USETEXTLINKS = 1
		STARTALLOPEN = 0
		USEFRAMES = 0
		USEICONS = 0
		WRAPTEXT = 1
		PRESERVESTATE = 1
		ICONPATH = 'javascripts/treeview/' 
		HIGHLIGHT = 1
		
		";
		if(is_null($categoryID)){
			$scriptInHead.="
				foldersTree = gFld('<b>Device Category</b>', \"javascript:document.forms[1].deviceSelected.value=0;document.forms[1].deviceCategSelected.value=0;document.forms[1].actionX.value=null;document.forms[1].submit();\");";
		}else{
			$queryCategoryName='SELECT Description FROM DeviceCategory WHERE PK_DeviceCategory=?';
			$resCategoryName=$dbADO->Execute($queryCategoryName,$categoryID);
			$rowCategoryName=$resCategoryName->FetchRow();
			$scriptInHead.="
				foldersTree = gFld('<b>".$rowCategoryName['Description']."</b>', \"javascript:document.forms[1].deviceSelected.value=0;document.forms[1].deviceCategSelected.value=".$categoryID.";document.forms[1].actionX.value=null;document.forms[1].submit();\");";
		}
		$scriptInHead.="
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
		$irgJsArray
		
		</script>
		";
		$out.=$scriptInHead;
		$formName=str_replace('operations/','',$section);
		$out.='
		<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']):'').'</div>	
		<div class="confirm"><B>'.stripslashes(@$_GET['msg']).'</B></div>	
		
		<form action="index.php" method="POST" name="'.$formName.'">
		<input type="hidden" name="section" value="'.$section.'">
		<input type="hidden" name="actionX" value="add">
		<input type="hidden" name="deviceCategSelected" value="'.$selectedDeviceCateg.'">
		<input type="hidden" name="deviceSelected" value="'.$selectedDevice.'">
		<input type="hidden" name="modelSelected" value="'.$selectedModel.'">
		<input type="hidden" name="allowAdd" value="'.(int)@$_REQUEST['allowAdd'].'">
		
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
							</select><br /><br />';
					if($boolManufacturer==1 && isset($_SESSION['userID'])){
						$out.='
							<input type="text" name="Manufacturer_Description" size="15" />
							<input type="submit" class="button" name="addManufacturer" value="Add"  />';
					}
					$out.=$firstColText.'
						</td>
						<td width="25%" align="center" valign="top">
							<span style="display:none;"><table border=0><tr><td><font size=-2><a style="font-size:7pt;text-decoration:none;color:#FFFFFF" href="http://www.treemenu.net/" target=_blank>JavaScript Tree Menu</a></font></td></tr></table></span>
							<span>
							<script>
								initializeDocument();						
							</script>
							<noscript>
							A tree for site navigation will open here if you enable JavaScript in your browser.
							</noscript>
							</span>
							<br /><br />';
						if($boolCategory==1){
							$out.='
							<input type="text" name="DeviceCategory_Description" size="15" />
							<input type="submit" class="button" name="addDeviceCategory"   value="Add '.($selectedDevice==0?' Top Level Child':' Child').'" />
							'.($selectedDevice!=0?'<input type="button" class="button" name="editDeviceCategory" value="Edit" onClick="javascript: windowOpen(\'index.php?section=editDeviceCategory&from='.$section.'&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'\',\'status=0,resizable=1,width=600,height=250,toolbars=true\');" />':'<input   type="submit" class="button" name="editDeviceCategory" value="Edit" disabled="disabled" />').'';
							getDeviceCategoryChildsNo($selectedDevice,$dbADO);					
							$childsToDelete = $GLOBALS['childsDeviceCategoryNo'];
							$out.='
							&nbsp;&nbsp;'.($selectedDevice!=0?'<input type="button" class="button" name="deteleDeviceCategory" value="Delete" onClick="javascript: if (confirm(\'Are you sure you want to delete this device category?'.($childsToDelete==1?'There is 1 child to delete!':($childsToDelete>0?'There are '.$childsToDelete.' childs to delete!':'')).'\')) self.location=\'index.php?section='.$section.'&manufacturers='.$selectedManufacturer.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'&actionX=del\';" />':'<input type="submit" class="button" name="deleteDeviceCategory"   value="Delete" disabled="disabled" />');
						}
						$out.='
						</td>
						<td width="25%" align="center"  valign="top" class="normaltext">
							<script>
								function checkEdit(targetSection) {
									if(targetSection==\'model\'){
										if(document.'.$formName.'.model.selectedIndex!=0){							
											self.location=\'index.php?section=editAVDevice&deviceID=\'+document.'.$formName.'.model[document.'.$formName.'.model.selectedIndex].value;
										}
									}else{
										if(document.'.$formName.'.irg.selectedIndex!=0){							
											self.location=\'index.php?section=irGroup&irgID=\'+document.'.$formName.'.irg[document.'.$formName.'.irg.selectedIndex].value;
										}
									
									}
								}
		
								function updateModelDetail(byItem) {
									objF=document.'.$formName.';
									eval("tmpFlag=objF."+byItem+".selectedIndex");
									eval("targetObj=objF."+byItem);
									eval("targetArray="+byItem+"sArray");
									if (tmpFlag!= "undefined") {
										document.getElementById(\'modelManuf\').innerHTML = \'<br />Manufacturer: \'+targetArray[targetObj[tmpFlag].value].manufacturerName;
										document.getElementById(\'modelDeviceDescription\').innerHTML = \'<br />Category: \'+targetArray[targetObj[tmpFlag].value].deviceDescription;
									} else {
										document.getElementById(\'modelManuf\').innerHTML = \'\';
										document.getElementById(\'modelDeviceDescription\').innerHTML = \'\';
									} 					
								}

							</script>
							<select name="model" id="model" 
									onChange="updateModelDetail(\'model\');"
									 size="10">
									<option value="" selected="selected">'.(($section=='deviceTemplatePicker')?'- Please select -':'All').'</option>
									'.$selectModels.'	
							</select>';
							if($returnValue==0){
								$out.='<br><input type="button" class="button" name="edit_DeviceTemplate" value="Edit Model" onClick="javascript:checkEdit(\'model\');" />';
							}else{
								$out.='<br><input type="button" class="button" name="pickDT" value="Add device" onClick="opener.location=\'index.php?section='.$_SESSION['from'].'&deviceTemplate=\'+document.'.$section.'.model[document.'.$section.'.model.selectedIndex].value+\'&action=add&add=1\';self.close();" />';
							}
							$out.='
							<hr />
							<em>* Green device templates are plug and play.<br>
								Red device templates are IR.</em><br>
						<b><span id="modelManuf"></span><span id="modelDeviceDescription"></span></b><br />
							
							';
							
							if ($selectedManufacturer!=0 && $selectedDevice!=0) {
								$disabledAdd=' ';
							} else {
								$disabledAdd=' disabled="disabled" ';
							}
					
							$disabled_str = "disabled";
				
				
				$out.='
						<br>
					<table width="100%">
						<tr>
							<th><h2>Infrared Groups</h2></th>
						</tr>
						<tr>
							<td align="center"><select name="irg" id="irg" 
									onChange="updateModelDetail(\'irg\');"
									 size="10">
									<option value="" selected="selected">'.(($section=='deviceTemplatePicker')?'- Please select -':'All').'</option>
									'.$selectIrgc.'	
							</select>
							<br>
							<input type="button" class="button" name="edit_irg" value="Edit IR Group" onClick="javascript:checkEdit(\'irg\');" />
							</td>
						</tr>				
					</table>
						</td>				
					</tr>';
				if($returnValue!=0){
					$out.='
					<tr>
						<td colspan="3">To add a device, choose the manufacturer and the category to see all models for you selection. Pick the model you want and click <B>"Add device"</B>.</td>
					</tr>';
				}
				if($boolDeviceTemplate==1){
					$out.='
					<tr>
						<td colspan="3">If your model is not listed, pick the manufacturer and the category and then type the model here: <input type="text" name="DeviceTemplate_Description" size="15"'.$disabledAdd.' />
							<input type="submit" class="button" name="addDeviceTemplate" value="Add"'. $disabledAdd .'  /></td>
					</tr>';
				}
				if($categoryID==$GLOBALS['rootAVEquipment']){
					$out.='
					<tr>
						<td colspan="3" class="normaltext">After you add the device you\'ll to choose the A/V properties button and then I/R codes.</td>
					</tr>';
				}
				$out.='
				</table>
							
		</form>
		';
	}elseif($actionX=='del'){
		deleteDeviceCategory($selectedDevice,$dbADO);
		$out.="
			<script>
				self.location='$redirectUrl&manufacturers=$selectedManufacturer&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&msg=Device category deleted.';
			</script>";
	} else {
		
		 $addDeviceCategory = @cleanString($_POST['addDeviceCategory']);
		 $deviceCategoryDesc = cleanString(@$_POST['DeviceCategory_Description']);
		 
		 $addDeviceTemplate = @cleanString($_POST['addDeviceTemplate']);
		 $DeviceTemplate_Desc = @cleanString($_POST['DeviceTemplate_Description']);
	
		 $addManufacturer = @cleanString($_POST['addManufacturer']);
		 $Manufacturer_Description = @cleanString($_POST['Manufacturer_Description']);	 
		
		 $deviceCategSelected = (int)$_POST['deviceCategSelected'];
		 $deviceSelected = (int)$_POST['deviceSelected'];
		 $manufacturerSelected = (int)$_POST['manufacturers'];
	
		 // process form only if user is logged
		 if (isset($_SESSION['userLoggedIn']) && $_SESSION['userLoggedIn']===true) {
	
			 // add new category
			 $justAddedNode = 0;
			 if (strstr($addDeviceCategory,'Add')) {
			 	if ($deviceCategoryDesc!='') {
			 		$queryInsertCategoryDesc = "insert into DeviceCategory(FK_DeviceCategory_Parent,Description,IsAV)
			 				values(?,?,0)";
			 		$deviceCategParent=($deviceSelected==0)?$categoryID:$deviceSelected;
			 		$dbADO->Execute($queryInsertCategoryDesc,array($deviceCategParent,$deviceCategoryDesc));	 			
			 		$justAddedNode = $deviceSelected;
			 		header("Location: $redirectUrl&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&justAddedNode=$justAddedNode");	 			 		
				}
			 }	
			 	 
			 //add new master device list
			 if (strstr($addDeviceTemplate,'Add')) {
			 	
			 	if ($DeviceTemplate_Desc!='') {
			 		if ($deviceSelected!=0 && $manufacturerSelected!=0) {	 			
			 			$queryInsertMasterDevice = 'INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer) values(?,?,?)';
			 			$res = $dbADO->Execute($queryInsertMasterDevice,array($DeviceTemplate_Desc,$deviceSelected,$manufacturerSelected));	 			
			 			$dtID=$dbADO->Insert_ID();

			 			if(in_array($deviceSelected,$GLOBALS['TVdevicesArray'])){
							$openTunerConfig='windowOpen(\'index.php?section=tunerConfig&categoryID='.$deviceSelected.'&dtID='.$dtID.'\',\'width=600,height=400,toolbars=true,scrollbars=1,resizable=1\')';
						}else
							$openTunerConfig='';
			 			
		 				$out.='
							<script>
								'.$openTunerConfig.'
								self.location="index.php?section='.$section.'&manufacturers='.$manufacturerSelected.'&deviceCategSelected='.$selectedDeviceCateg.'&deviceSelected='.$selectedDevice.'&model='.$selectedModel.'&allowAdd='.$boolDeviceTemplate.'&justAddedNode='.$justAddedNode.'";
							</script>';
		 			
			 		}	 			 		
			 	}
			 }
			 
			 // add new manufacturer
			 if (strstr($addManufacturer,'Add')){
			 	if ($Manufacturer_Description!='') {
		 			$queryInsertManufacturer = 'INSERT INTO Manufacturer (Description) values(?)';
		 			$res = $dbADO->Execute($queryInsertManufacturer,array($Manufacturer_Description));	 			
		 			header("Location: $redirectUrl&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&justAddedNode=$justAddedNode");
		 		}	 			 		
			 }
			$out.="
				<script>
					self.location='$redirectUrl&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&justAddedNode=$justAddedNode';
				</script>";
		}else{
			$out.="
				<script>
					self.location='$redirectUrl&manufacturers=$manufacturerSelected&deviceCategSelected=$selectedDeviceCateg&deviceSelected=$selectedDevice&model=$selectedModel&allowAdd=$boolDeviceTemplate&error=Please login if you want to change device templates.';
				</script>";
		}
	}
	return $out;
}

function getChilds($parentID,$dbADO) {
	
	$selectDevicesFromCategories = "SELECT * FROM DeviceCategory WHERE FK_DeviceCategory_Parent = {$parentID} ORDER BY Description";
		$rs2 = $dbADO->_Execute($selectDevicesFromCategories);
		$jsTree='';
			while ($row2 = $rs2->FetchRow()) {		
				$jsTree.= '
					auxS'.$row2['PK_DeviceCategory'].' = insFld(auxS'.$parentID.', gFld("'.$row2['Description'].' #'.$row2['PK_DeviceCategory'].'", "javascript:document.forms[1].deviceCategSelected.value='.$parentID.';document.forms[1].deviceSelected.value='.$row2['PK_DeviceCategory'].';document.forms[1].actionX.value=null;document.forms[1].submit();"))
					auxS'.$row2['PK_DeviceCategory'].'.xID = '.$row2['PK_DeviceCategory'].';
				';
				$jsTree.=getChilds($row2['PK_DeviceCategory'],$dbADO);
			}

	return $jsTree;
}


function deleteDeviceCategory($categoryID,$dbADO)
{
	getDeviceCategoryChildsArray($categoryID,$dbADO);

	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$categoryID;

	if ((int)$categoryID!=0) {
		$deleteObjFromDevice = 'DELETE FROM DeviceCategory WHERE PK_DeviceCategory in ('.join(",",$GLOBALS['childsDeviceCategoryArray']).')';
		$query = $dbADO->_Execute($deleteObjFromDevice);
		
		$deleteDT = 'DELETE FROM DeviceTemplate WHERE FK_DeviceCategory in ('.join(",",$GLOBALS['childsDeviceCategoryArray']).')';
		$query = $dbADO->_Execute($deleteDT);

		return 1;
	} else {
		return 0;
	}
}


function pulldownFromArray($valuesArray,$name,$selectedValue,$extra='')
{
//	if(count($valuesArray)==0)
//		return null;
	$out='<select name="'.$name.'" "'.$extra.'">
			<option value="0">- Please select -</option>';
	foreach ($valuesArray AS $key=>$value){
		$out.='<option value="'.$key.'" '.(($key==$selectedValue)?'selected':'').'>'.$value.'</option>';
	}
	$out.='</select>';
	return $out;
}

function generatePullDown($name,$tableName,$valueField,$labelField,$selectedValue,$dbADO,$filterTable='',$jsEvents='')
{
	$pullDown='
		<select name="'.$name.'" '.$jsEvents.'>	
			<option value="0">- Please select -</option>';
	$query="SELECT $tableName.$valueField,$tableName.$labelField FROM $tableName $filterTable ORDER BY $labelField ASC";
	$res=$dbADO->Execute($query);
	while($row=$res->FetchRow()){
		$pullDown.='<option value="'.$row[$valueField].'" '.(($row[$valueField]==$selectedValue)?'selected':'').'>'.$row[$labelField].'</option>';
	}
	$pullDown.='</select>';
	return $pullDown;
}

function getCheckedDeviceCommandGroup($deviceTemplate,$deviceCategory,$dbADO)
{
	$querySelCheckedCommandGroups = 'SELECT FK_DeviceCommandGroup FROM DeviceTemplate_DeviceCommandGroup WHERE  FK_DeviceTemplate = ?';
	$resSelCheckedCommandGroups = $dbADO->Execute($querySelCheckedCommandGroups,$deviceTemplate);
	$selCheckedCommandsGroups = array();
	if ($resSelCheckedCommandGroups) {
		while ($rowSelCheckedCommandGroups = $resSelCheckedCommandGroups->FetchRow()) {
			$selCheckedCommandsGroups[]=$rowSelCheckedCommandGroups['FK_DeviceCommandGroup'];
		}
	}

	if (!is_array($selCheckedCommandsGroups)) {
		$selCheckedCommandsGroups=array();
		$selCheckedCommandsGroups[]=0;
	}
	if (count($selCheckedCommandsGroups)==0) {
		$selCheckedCommandsGroups[]=0;
	}

	$query = "
		SELECT  ".$dbADO->IfNull('FK_DeviceCategory_Parent','0')." AS parent 
		FROM DeviceCategory
		WHERE PK_DeviceCategory = ?";
	$deviceParent=0;
	$res = $dbADO->Execute($query,array($deviceCategory));
	if ($res) {
		while ($row = $res->FetchRow()) {
			$deviceParent=$row['parent'];
		}
	}

	$deviceParent=(int)$deviceParent;
	$deviceCG = array();
	$query = "
		SELECT DeviceCommandGroup.* 
		FROM DeviceCommandGroup
		INNER JOIN DeviceCategory on FK_DeviceCategory = PK_DeviceCategory
		WHERE 
			FK_DeviceCategory in ($deviceParent,$deviceCategory) OR
			PK_DeviceCommandGroup in (".join(",",$selCheckedCommandsGroups).") 
		ORDER BY DeviceCommandGroup.Description Asc	";
	$resCommands = $dbADO->_Execute($query);
				
	$groups = array();
	if ($resCommands) {
		while ($row = $resCommands->FetchRow()) {
			$groups[$row['PK_DeviceCommandGroup']]['checked']=(in_array($row['PK_DeviceCommandGroup'],$selCheckedCommandsGroups))?1:0;
			$groups[$row['PK_DeviceCommandGroup']]['Description']=$row['Description'];
		}
	}
	
	$resN=$dbADO->Execute('SELECT * FROM DeviceCommandGroup WHERE FK_DeviceCategory IS NULL');
	while($rowN=$resN->FetchRow()){
		$groups[$rowN['PK_DeviceCommandGroup']]['checked']=(in_array($rowN['PK_DeviceCommandGroup'],$selCheckedCommandsGroups))?1:0;
		$groups[$rowN['PK_DeviceCommandGroup']]['Description']=$rowN['Description'];
	}
	
	return $groups;
}

?>