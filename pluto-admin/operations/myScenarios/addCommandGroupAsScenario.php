<?php
function addCommandGroupAsScenario($output,$dbADO) {
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */

	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$arrayID = (int)$_REQUEST['arrayID'];
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	if ($action=='form') {
		
		
		
		
			
			$out.='
			<script>
				function windowOpen(locationA,attributes) {
					window.open(locationA,\'\',attributes);
				}
			</script>
			<form action="index.php" method="POST" name="addCommandGroupAsScenario">
				<input type="hidden" name="section" value="addCommandGroupAsScenario">
				
				<input type="hidden" name="action" value="add">	
				<input type="hidden" name="from" value="'.$from.'">	
				<input type="hidden" name="arrayID" value="'.$arrayID.'">
				<h2>Add My Scenario</t2>
				<table>
						<tr><td>Description</td>
						<td><input type="text" size="20" name="description" value="'.@$_SESSION['addCommandGroupAsScenario']['Description'].'"></td></tr>
						<tr><td>Scenario Type</td><td>
								<select name="arrayType">';
			
			$queryArrays = "SELECT * FROM Array Where CommandGroup = 1 order by Description Asc";
			$resArrays = $dbADO->Execute($queryArrays);
			
						while ($rowArray=$resArrays->FetchRow()) {
							$out.='<option '.($rowArray['PK_Array']==$arrayID?" selected='selected' ":'').' value="'.$rowArray['PK_Array'].'">'.$rowArray['Description'].'</option>';
						}
						
			$out.='
								</select>
						</td></tr>
						<tr>
					<td>Design Obj:</td>
					<td>
						'.(@(int)$_SESSION['addCommandGroupAsScenario']['designObjID']!=0
								?'You have selected : <b>'.$_SESSION['addCommandGroupAsScenario']['designObjName'].'</b>&nbsp;&nbsp;<br />'
								:(
									isset($_GET['msg'])
									?'<b>'.strip_tags($_GET['msg']).'</b>'
									:'')
								 ).' 
								 <input type="text" name="designObjID" size="20" value="'.(isset($_SESSION['addCommandGroupAsScenario']['designObjID']) && $_SESSION['addCommandGroupAsScenario']['designObjID'] != 0 ?$_SESSION['addCommandGroupAsScenario']['designObjID']:'').'">
								 <input type="submit" name="searchCommandGroup" value="Search [...]" >
					</td>
				</tr>
				<tr><td>Icon</td><td>
						<select name="icon">';
	
							$queryIcons = "SELECT * FROM Icon order by Description Asc";
							$resIcons = $dbADO->Execute($queryIcons);
										while ($rowIcons=$resIcons->fetchRow()) {
											$out.='<option value="'.$rowIcons['PK_Icon'].'">'.$rowIcons['Description'].'</option>';
										}
						
							$out.='
						</select>
				</td>
				</tr>
				<tr>
					<td>Template</td><td>
						<select name="template">';
	
							$queryTemplates = "SELECT * FROM Template order by Description Asc";
							$resTemplates = $dbADO->Execute($queryTemplates);
										while ($rowTemplates=$resTemplates->fetchRow()) {
											$out.='<option value="'.$rowTemplates['PK_Template'].'">'.$rowTemplates['Description'].'</option>';
										}
						
							$out.='
						</select>
					</td>
				</tr>
				<tr>
					<td>'.(($arrayID==$GLOBALS['ArrayIDForMedia'])?'Entertainment area':'Rooms').'</td>
					<td>';
					if($arrayID==$GLOBALS['ArrayIDForMedia']){
						$queryEntArea='
							SELECT EntertainArea.* FROM EntertainArea
								INNER JOIN Room ON FK_Room=PK_Room
							WHERE 
								FK_Installation=?
							ORDER BY EntertainArea.Description ASC';
						$resEntArea=$dbADO->Execute($queryEntArea,$installationID);
						$displayedEntArea=array();
						while($rowEntArea=$resEntArea->FetchRow()){
							$displayedEntArea[]=$rowEntArea['PK_EntertainArea'];
							$out.='<input type="checkbox" value="1" name="entArea_'.$rowEntArea['PK_EntertainArea'].'"> '.$rowEntArea['Description'].'<br>';
						}
						$out.='<input type="hidden" name="displayedEntArea" value="'.join(",",$displayedEntArea).'">';
					}else{
						$queryRooms='SELECT Room.* FROM Room WHERE FK_Installation=? ORDER BY Room.Description ASC';
						$resRooms=$dbADO->Execute($queryRooms,$installationID);
						$displayedRooms=array();
						while($rowRooms=$resRooms->FetchRow()){
							$displayedRooms[]=$rowRooms['PK_Room'];
							$out.='<input type="checkbox" value="1" name="room_'.$rowRooms['PK_Room'].'"> '.$rowRooms['Description'].'<br>';
						}
						$out.='<input type="hidden" name="displayedRooms" value="'.join(",",$displayedRooms).'">';
					}
				$out.='</td>
				</tr>
				<tr>
					<td colspan="2"><input type="submit" name="submitX" value="Save"></td>
				</tr>
				';
						
				$out.='
				</table>	
			</form>
			<script>
		 		var frmvalidator = new formValidator("addCommandGroupAsScenario");
 				frmvalidator.addValidation("description","req","Please enter a package description");			
				//frmvalidator.addValidation("designObjID","req","Please enter a design obj number!");			
	 			//frmvalidator.addValidation("icon","dontselect=0","Please select an Icon!");			
			</script>
			';
		
		
	} else {
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		
		$description = $_SESSION['addCommandGroupAsScenario']['Description'] = cleanString($_POST['description']);
	
		$designObjID = cleanInteger(@$_POST['designObjID']);
		$_SESSION['addCommandGroupAsScenario']['designObjID'] = 0;
		$_SESSION['addCommandGroupAsScenario']['designObjName'] = '';
		
		
		$querySelectCommandGroup = "
							select  PK_DesignObj,Description from DesignObj
							where PK_DesignObj = ?";
		$rs = $dbADO->Execute($querySelectCommandGroup,array($designObjID));
		while ($row = $rs->FetchRow()) {
				$_SESSION['addCommandGroupAsScenario']['designObjID'] = $row['PK_DesignObj'];
				$_SESSION['addCommandGroupAsScenario']['designObjName'] = $row['Description'];
		}
		$rs->Close();
		
			
		if (isset($_POST['searchCommandGroup'])) {
			if ($_SESSION['addCommandGroupAsScenario']['designObjID'] !=0) {
				header("Location: index.php?section=addCommandGroupAsScenario&from=$from&arrayID=$arrayID");
				exit();
			} else {
				header("Location: index.php?section=addCommandGroupAsScenario&from=$from&arrayID=$arrayID&msg=Invalid+ID");
				exit();				
			}
		}
		
		
		
		//die();				
		if ($canModifyInstallation) {
			
			
			$arrayID = cleanInteger($_POST['arrayType']);
			$designObjID = $_SESSION['addCommandGroupAsScenario']['designObjID'];
			$iconID = cleanInteger(isset($_POST['icon'])?$_POST['icon']:0);
			$templateID = cleanInteger($_POST['template']);
						
			$insertCommandGroup = "INSERT INTO CommandGroup (Description,FK_Array,FK_DesignObj,FK_Icon,FK_Template,FK_Installation)
				VALUES(?,?,?,?,?,?)			
			";

			$res=$dbADO->Execute($insertCommandGroup,array($description,$arrayID,$designObjID,$iconID,$templateID,$installationID));
			$PK_CommandGroup=$dbADO->Insert_ID();
			
			if($arrayID==$GLOBALS['ArrayIDForMedia']){
				$displayedEntArea =cleanString($_POST['displayedEntArea']);
				if($displayedEntArea!=''){
					$EntAreaArray=explode(',',$displayedEntArea);
					foreach ($EntAreaArray AS $value){
						if(isset($_POST['entArea_'.$value]) && $_POST['entArea_'.$value]==1){
							$insertCommandGroupEntArea='
								INSERT INTO CommandGroup_EntertainArea 
									(FK_CommandGroup, FK_EntertainArea,Sort)
								VALUES
									(?,?,?)';
							$dbADO->Execute($insertCommandGroupEntArea,array($PK_CommandGroup,$value),$PK_CommandGroup);
						}
					}
				}
			}else{
			$displayedRooms =cleanString($_POST['displayedRooms']);
				if($displayedRooms!=''){
					$roomsArray=explode(',',$displayedRooms);
					foreach ($roomsArray AS $value){
						if(isset($_POST['room_'.$value]) && $_POST['room_'.$value]==1){
							$insertCommandGroupRoom='
								INSERT INTO CommandGroup_Room 
									(FK_CommandGroup, FK_Room,Sort)
								VALUES
									(?,?,?)';
							$dbADO->Execute($insertCommandGroupRoom,array($PK_CommandGroup,$value,$PK_CommandGroup));
						}
					}
				}
			}
			
			$out.="
			<script>
				alert('My Scenario Added!');
			    //opener.document.forms.{$from}.action.value='form';
				//opener.document.forms.{$from}.submit();
				opener.location.reload();
				opener.top.frames[0].location.reload();
				self.close();				
			</script>
			";
			$_SESSION['addCommandGroupAsScenario']=array();
			
		}
		
		 
		
	}
		
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();
}
?>