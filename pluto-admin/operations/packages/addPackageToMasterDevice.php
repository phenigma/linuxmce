<?php
function addPackageToMasterDevice($output,$dbADO) {
//	$dbADO->debug=true;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (isset($_REQUEST['deviceID']))?(int)$_REQUEST['deviceID']:0;
	$PK_Package = (isset($_REQUEST['PK_Package']))?cleanInteger($_REQUEST['PK_Package']):0;

	// get array for operating system pull down
	$queryOperatingSystem='SELECT * FROM OperatingSystem ORDER BY Description ASC';
	$rs = $dbADO->Execute($queryOperatingSystem);
	$arrayIdOperatingSystem=array();
	$arrayDescriptionOperatingSystem=array();
	while ($row = $rs->FetchRow()) {
		$arrayIdOperatingSystem[]=$row['PK_OperatingSystem'];
		$arrayDescriptionOperatingSystem[]=$row['Description'];
	}
	$rs->Close();
	
	// get array for distro pull down
	$arrayIdDistro=array();
	$arrayDescriptionDistro=array();
	$queryDistroOperatingSystem = "
					SELECT  Distro.*, OperatingSystem.Description AS OS FROM Distro,OperatingSystem 
					WHERE Distro.FK_OperatingSystem= OperatingSystem.PK_OperatingSystem ORDER BY Distro.Description ASC";
	$rs = $dbADO->_Execute($queryDistroOperatingSystem);
	while ($row = $rs->FetchRow()) {
		$arrayIdDistro[]=$row['PK_Distro'];
		$arrayDescriptionDistro[]=$row['Description']." / ".$row['OS'];
	}
	$rs->Close();
	
	// get array for  RepositorySource pull down
	$arrayRepositoryID=array();
	$arrayRepository=array();
	$queryRepository = '
		SELECT  
			RepositorySource.*,
			RepositoryType.Description AS RepositoryTypeName 
		FROM RepositorySource 
			INNER JOIN RepositoryType ON 
				FK_RepositoryType=PK_RepositoryType
		ORDER BY Description ASC';
	$rs = $dbADO->_Execute($queryRepository);
	while ($row = $rs->FetchRow()) {
		$arrayRepositoryID[]=$row['PK_RepositorySource'];
		$arrayRepository[]=$row['Description']." / ".$row['RepositoryTypeName'];
	}
	$rs->Close();

	
	if ($action=='form') {
		$DataFormValidation='';
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
			function disableSource()
			{
				if(document.forms[0].isSourceCode.checked)
					document.forms[0].SourceCode.disabled="disabled";
				else
					document.forms[0].SourceCode.disabled="";
			}
		</script>
		<form action="index.php" method="post" name="addPackageToMasterDevice">
		<input type="hidden" name="section" value="addPackageToMasterDevice">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="PK_Package" value="'.$PK_Package.'">
			<table>			
				<tr>
					<td colspan="2">
					<table cellpadding="3">';
					$querySelectPackage= '
						SELECT Package.* 
							FROM Package 
						LEFT JOIN  DeviceTemplate on FK_Package  = PK_Package
							WHERE PK_Package = ?';
				$rs = $dbADO->Execute($querySelectPackage,array($PK_Package));

				$rowPackage = $rs->FetchRow(); 
				$out.='<tr>
								<td align="right">Description: </td>
								<td><input type="text" name="Description" value="'.$rowPackage['Description'].'"> (#'.$PK_Package.')</td>
						   </tr>
						   <tr>
								<td align="right">This package contain source code.</td>
								<td align="left"><input name="isSourceCode" type="checkbox" value="1" '.(($rowPackage['IsSource']==1)?'checked':'').' '.(($PK_Package!=0)?'onClick="javascript:this.form.submit();"':'OnClick="javascript:disableSource();"').'></td>
						  </tr>
						   <tr>
								<td align="right">The source code for this package is:</td>
								<td><select name="SourceCode" '.(($rowPackage['IsSource']==1)?'disabled':'').'>
										<option value="0">-Please select</option>';
				$querySourceCode='SELECT * FROM Package WHERE IsSource=1 ORDER BY Description ASC';
				$resSourceCode=$dbADO->execute($querySourceCode);
				while($rowSourceCode=$resSourceCode->FetchRow()){
					$out.='<option value="'.$rowSourceCode['PK_Package'].'" '.(($rowSourceCode['PK_Package']==$rowPackage['FK_Package_Sourcecode'])?'selected':'').'>'.$rowSourceCode['Description'].'</option>';
				}
				$out.='</select><input type="Submit" name="submitX" value="Select"></td>
						 </tr>
						<tr>
							<td align="right">Package Type</td>
							<td><select name="PackageType">
										<option value="0">-Please select</option>';
				$queryPackageType='SELECT * FROM PackageType ORDER BY Description ASC';
				$resPackageType=$dbADO->execute($queryPackageType);
				while($rowPackageType=$resPackageType->FetchRow()){
					$out.='<option value="'.$rowPackageType['PK_PackageType'].'" '.(($rowPackageType['PK_PackageType']==$rowPackage['FK_PackageType'])?'selected':'').'>'.$rowPackageType['Description'].'</option>';
				}
				$out.='</select></td>
						</tr>';
				if($PK_Package!=0){
					$out.='	 <tr>
								<td colspan="2">
								<fieldset><legend> Packages this depends on </legend>';
					$out.='		<table>';
					$queryPackagesDependsOn='
						SELECT * FROM Package
							INNER JOIN Package_Package ON Package_Package.FK_Package_DependsOn=Package.PK_Package
						WHERE FK_Package=?';
					$resPackagesDependsOn=$dbADO->Execute($queryPackagesDependsOn,$PK_Package);
					if($resPackagesDependsOn->RecordCount()==0)
						$out.='No records.';
					$displayedPackages=array();
					$displayedPackages[]=$PK_Package;
					while($rowPackagesDependsOn=$resPackagesDependsOn->FetchRow()){
						$displayedPackages[]=$rowPackagesDependsOn['FK_Package_DependsOn'];
						$out.='<tr>
									<td>'.$rowPackagesDependsOn['Description'].'</td>
									<td><input type="checkbox" name="OnlyToBuild_'.$rowPackagesDependsOn['PK_Package'].'" '.(($rowPackagesDependsOn['OnlyToBuild']==1)?'checked':'').' onClick="javascript:this.form.submit();" value="1"> Building from source</td>
									<td><input type="submit" name="del_'.$rowPackagesDependsOn['PK_Package'].'" value="Delete"></td>
						    	</tr>';
					}
					$out.='</table> 
					Add new package that this depends on: <select name="newPackageDependsOn">
						<option value="0">-Please select-</option>';
					$filterRemainingPackages=(count($displayedPackages)>0)?' WHERE PK_Package NOT IN ('.join(',',$displayedPackages).')':'';
					$queryRemainingPackages='
						SELECT * 
							FROM Package 
								'.$filterRemainingPackages.'
							ORDER BY Description ASC';
					$resRemainingPackages=$dbADO->Execute($queryRemainingPackages);
					while($rowRemainingPackages=$resRemainingPackages->FetchRow()){
						$out.='<option value="'.$rowRemainingPackages['PK_Package'].'">'.$rowRemainingPackages['Description'].'</option>';
					}
					$out.='</select>
						<input type="Submit" name="submitX" value="Add">
						</fieldset>
						<input type="hidden" name="displayedPackages" value="'.join(",",$displayedPackages).'">
						</td>
					</tr>
					<tr>
						<td colspan="2">
						<fieldset><legend>Sources for package</legend>
						<table>';
					$compatArray=array();
					$querySources='SELECT * FROM Package_Source WHERE FK_Package=?';
					$resSources=$dbADO->execute($querySources,$PK_Package);
					$displayedSources=array();
					while($rowSource=$resSources->FetchRow()){
						$displayedSources[]=$rowSource['PK_Package_Source'];
						$DataFormValidation.= '
							frmvalidator.addValidation("source_'.$rowSource['PK_Package_Source'].'","req","Please enter a source name!");';
	
						$out.='
							<tr bgcolor="#F0F3F8">
								<td>Name</td>
								<td>Repository Source</td>
								<td>Repository</td>
								<td>Version</td>
								<td>Parms</td>
								<td>&nbsp;</td>
							</tr>
							<tr bgcolor="#F0F3F8">
								<td><input type="text" name="source_'.$rowSource['PK_Package_Source'].'" value="'.$rowSource['Name'].'"></td>
								<td><select name="repositorySource_'.$rowSource['PK_Package_Source'].'">
								<option value="0">-Any-</option>';
						foreach($arrayRepository as $key => $value){
							$out.='<option value="'.$arrayRepositoryID[$key].'" '.(($arrayRepositoryID[$key]==$rowSource['FK_RepositorySource'])?'selected':'').'>'.$value.'</option>';
						}
						$out.='	</select>
								</td>
								<td><input type="text" name="repository_'.$rowSource['PK_Package_Source'].'" value="'.$rowSource['Repository'].'"></td>
								<td><input type="text" name="version_'.$rowSource['PK_Package_Source'].'" value="'.$rowSource['Version'].'"></td>
								<td><input type="text" name="parms_'.$rowSource['PK_Package_Source'].'" value="'.$rowSource['Parms'].'"></td>
								<td><input type="submit" name="deleteSource_'.$rowSource['PK_Package_Source'].'" value="Delete source"></td>
							</tr>
						<input type="hidden" name="displayedSources" value="'.join(",",$displayedSources).'">
						';
					$querySourceCompatibility='SELECT * FROM Package_Source_Compat WHERE FK_Package_Source=?';
					$resSourceCompatibility=$dbADO->Execute($querySourceCompatibility,$rowSource['PK_Package_Source']);
					while($rowCompatibility=$resSourceCompatibility->fetchRow()){
						$compatArray[]=$rowCompatibility['PK_Package_Source_Compat'];
						$out.='
							<tr>
								<td>&nbsp;</td>
								<td><select name="OperatingSystem_'.$rowCompatibility['PK_Package_Source_Compat'].'">
									<option value="0">-Any-</option>';
						foreach($arrayDescriptionOperatingSystem as $key => $value){
							$out.='<option value="'.$arrayIdOperatingSystem[$key].'" '.(($arrayIdOperatingSystem[$key]==$rowCompatibility['FK_OperatingSystem'])?'selected':'').'>'.$value.'</option>';
						}
						$out.='	</select></td>
								<td><select name="distro_'.$rowCompatibility['PK_Package_Source_Compat'].'">
									<option value="0">-Any-</option>';
						foreach($arrayDescriptionDistro as $key => $value){
							$out.='<option value="'.$arrayIdDistro[$key].'" '.(($arrayIdDistro[$key]==$rowCompatibility['FK_Distro'])?'selected':'').'>'.$value.'</option>';
						}
//						echo $rowCompatibility['PK_Package_Source_Compat'];
						$out.='	</select></td>
								<td><input type="checkbox" name="mustBuild_'.$rowCompatibility['PK_Package_Source_Compat'].'" '.(($rowCompatibility['MustBuildFromSource']==1)?'checked':'').' value="1">Must build from source</td>
								<td colspan="2"><textarea name="comments_'.$rowCompatibility['PK_Package_Source_Compat'].'">'.$rowCompatibility['Comments'].'</textarea></td>
								<td><input type="submit" name="deleteCompatibility_'.$rowCompatibility['PK_Package_Source_Compat'].'" value="Delete compat"></td>
							</tr>
						';
					}
					$out.='<tr>
								<td>
									<input type="submit" name="addCompatibility_'.$rowSource['PK_Package_Source'].'" value="Add other compatibility"><br>
								</td>
								<td colspan="6" align="center"><input type="submit" name="save" value="Save"></td>
						   </tr>
					';
					
				}
	
				$out.='		<tr>
								<td colspan="7">
									<input type="submit" name="addSource" value="Add other source">
								</td>
							</tr>
						</table>
						</fieldset>
						</td>
					</tr>
		
					<tr>
						<td colspan="2"><fieldset><legend>Requested directories and files</legend>';
				$queryDirectoryPackage='
					SELECT * FROM Package_Directory
						INNER JOIN Directory ON FK_Directory=PK_Directory
					WHERE FK_Package=? ORDER BY Description ASC';
				$resDirectoryPackage=$dbADO->Execute($queryDirectoryPackage,array($PK_Package));

				if($resDirectoryPackage->RecordCount()==0)
					$out.='No Directory selected';
				$displayedDirectory=array();
				$displayedPackageDirectory=array();
				$displayedPackageDirectoryFiles=array();
				while ($row = $resDirectoryPackage->FetchRow()) {
					$displayedDirectory[]=$row['PK_Directory'];
					$displayedPackageDirectory[]=$row['PK_Package_Directory'];
					$out.='<fieldset><legend>'.$row['Description'].' <select name="packageDirectoryOS_'.$row['PK_Package_Directory'].'">
									<option value="0">-Any-</option>';
						foreach($arrayDescriptionOperatingSystem as $key => $value){
							$out.='<option value="'.$arrayIdOperatingSystem[$key].'" '.(($arrayIdOperatingSystem[$key]==$row['FK_OperatingSystem'])?'selected':'').'>'.$value.'</option>';
						}
						$out.='	</select>
						<select name="packageDirectoryDistro_'.$row['PK_Package_Directory'].'">
									<option value="0">-Any-</option>';
						foreach($arrayDescriptionDistro as $key => $value){
							$out.='<option value="'.$arrayIdDistro[$key].'" '.(($arrayIdDistro[$key]==$row['FK_Distro'])?'selected':'').'>'.$value.'</option>';
						}
						$out.='	</select> Path:
						<input type="text" name="packageDirectoryPath_'.$row['PK_Package_Directory'].'" value="'.$row['Path'].'">
						Alternate input path: <input type="text" name="packageDirectoryInputPath_'.$row['PK_Package_Directory'].'" value="'.$row['InputPath'].'">
						Flip source <input type="checkbox" name="packageDirectoryFlipSource_'.$row['PK_Package_Directory'].'" value="1" '.(($row['FlipSource']=='1')?'checked':'').'>
						<input type="submit" name=da value="Save">
						</legend>';
					
					$queryDirectoryFiles='SELECT * FROM Package_Directory_File WHERE FK_Package_Directory=?';
					$resDirectoryFiles=$dbADO->Execute($queryDirectoryFiles,array($row['PK_Package_Directory']));
					$out.='<table width="550">';
					if($resDirectoryFiles->RecordCount()==0)
						$out.='<tr>
									<td colspan="4">No files selected.</td>
								</tr>';
					while ($rowDirectoryFiles = $resDirectoryFiles->FetchRow()){
						$displayedPackageDirectoryFiles[]=$rowDirectoryFiles['PK_Package_Directory_File'];
						$out.='
						<tr bgcolor="#F0F3F8">
							<td><input type="text" name="pdfFile_'.$rowDirectoryFiles['PK_Package_Directory_File'].'" value="'.$rowDirectoryFiles['File'].'"></td>
							<td><select name="pdfOS_'.$rowDirectoryFiles['PK_Package_Directory_File'].'">
									<option value="0">-Any-</option>';
						foreach($arrayDescriptionOperatingSystem as $key => $value){
							$out.='<option value="'.$arrayIdOperatingSystem[$key].'" '.(($arrayIdOperatingSystem[$key]==$rowDirectoryFiles['FK_OperatingSystem'])?'selected':'').'>'.$value.'</option>';
						}
						$out.='	</select></td>
						<td><select name="pdfDistro_'.$rowDirectoryFiles['PK_Package_Directory_File'].'">
								<option value="0">-Any-</option>';
						foreach($arrayDescriptionDistro as $key => $value){
							$out.='<option value="'.$arrayIdDistro[$key].'" '.(($arrayIdDistro[$key]==$rowDirectoryFiles['FK_Distro'])?'selected':'').'>'.$value.'</option>';
						}
						$out.='	</select></td>
							<td><textarea name="pdfMakeCommand_'.$rowDirectoryFiles['PK_Package_Directory_File'].'" rows="1">'.$rowDirectoryFiles['MakeCommand'].'</textarea></td>
							<td align="right" bgcolor="#F0F3F8"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editDirectoryFile&from=addPackageToMasterDevice&deviceID='.$deviceID.'&Package_Directory_File='.$rowDirectoryFiles['PK_Package_Directory_File'].'\',\'status=0,resizable=1,width=400,height=350,toolbars=true\');">Edit</a> <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deleteDirectoryFile&from=addPackageToMasterDevice&deviceID='.$deviceID.'&Package_Directory='.$row['PK_Package_Directory'].'&oldFile='.$rowDirectoryFiles['File'].'\',\'status=0,resizable=1,width=100,height=100,toolbars=true\');">Delete</a></td>		
						</tr>';
					}
	
					$out.='
						<tr>
							<td colspan="4"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addDirectoryFile&from=addPackageToMasterDevice&deviceID='.$deviceID.'&Package_Directory='.$row['PK_Package_Directory'].'\',\'status=0,resizable=1,width=500,height=350,toolbars=true\');">Add file</a></td>
						</tr>
					</table>';
					$out.='</fieldset>';
				}
							
						$out.='
							<select name="FK_Directory">
							<option value="0">-please select-</option>
							';
							$queryDirectory = "
								SELECT  Directory.* FROM Directory ORDER BY Directory.Description ASC";
							$rs = $dbADO->_Execute($queryDirectory);
							while ($row = $rs->FetchRow()) {
									$out.='<option value="'.$row['PK_Directory'].'">'.$row['Description']."</option>";
							}
							$rs->Close();
						$out.='
							</select> <input type="Submit" name="submitX" value="Select"> <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addDirectory&from=addPackageToMasterDevice&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbars=true\');">Add Directory</a>';
						$out.='</fieldset></td>
					</tr>
					<input type="hidden" name="displayedPackageDirectory" value="'.join(",",$displayedPackageDirectory).'">
					<input type="hidden" name="displayedPackageDirectoryFiles" value="'.join(",",$displayedPackageDirectoryFiles).'">
					<input type="hidden" name="compatArray" value="'.join(",",$compatArray).'">';
				}
				$out.='<tr>
					<td colspan="2" align="center"><input type="submit" name="saveClose" value="Save & Close"> <input type="button" name="cancelClose" value="Cancel & Close" onClick="javascript:self.close()"></td>
				</tr>
			</table>
		<script>
		 	var frmvalidator = new formValidator("addPackageToMasterDevice");
 			frmvalidator.addValidation("Description","req","Please enter a description");
			'.$DataFormValidation.'
		</script>
	
		</form>
		';
	} else {
		if ((int)$PK_Package!=0) {
			
			$isSourceCode = isset($_REQUEST['isSourceCode'])?cleanInteger($_REQUEST['isSourceCode']):0;
			$description=$_POST['Description'];
			$FK_Package_Sourcecode=($isSourceCode==0)?((@$_POST['SourceCode']!=0)?(int)@$_POST['SourceCode']:NULL):NULL;
			$displayedPackagesArray=explode(",",@$_POST['displayedPackages']);
			$newPackageDependsOn=(int)$_POST['newPackageDependsOn'];
			$displayedSourcesArray=explode(",",@$_POST['displayedSources']);
			$compatArray=explode(",",@$_POST['compatArray']);
			$FK_Directory = cleanInteger(@$_REQUEST['FK_Directory']);
			$FK_PackageType=(@$_POST['PackageType'])?(int)@$_POST['PackageType']:NULL;
			
			$updatePackage='
				UPDATE Package 
					SET Description=?, FK_Package_Sourcecode =?, IsSource=?, FK_PackageType=?
				WHERE PK_Package=?';
			$dbADO->Execute($updatePackage,array($description, $FK_Package_Sourcecode,$isSourceCode,$FK_PackageType,$PK_Package));
			
			foreach($displayedPackagesArray AS $value){
				$OnlyToBuild=(isset($_POST['OnlyToBuild_'.$value]))?$_POST['OnlyToBuild_'.$value]:0;
				if(isset($_POST['del_'.$value])){
					$deletePackagePackage='DELETE FROM Package_Package WHERE FK_Package=? AND FK_Package_DependsOn=?';
					$dbADO->Execute($deletePackagePackage,array($PK_Package,$value));
				}else{
					$updatePackagePackage='UPDATE Package_Package SET OnlyToBuild=? WHERE FK_Package=? AND FK_Package_DependsOn=?';
					$dbADO->Execute($updatePackagePackage,array($OnlyToBuild,$PK_Package,$value));
				}
			}
			
			if($newPackageDependsOn!=0){
				$insertPackagePackage='INSERT INTO Package_Package (FK_Package, FK_Package_DependsOn,OnlyToBuild) VALUES (?,?,?)';
				$dbADO->Execute($insertPackagePackage,array($PK_Package,$newPackageDependsOn,0));
			}
			
			if(isset($_POST['addSource'])){
				// add new source in table Package_Source
				$insertPackageSource='INSERT INTO Package_Source (FK_Package) VALUES (?)';
				$dbADO->Execute($insertPackageSource,$PK_Package);
			}
			
			foreach($compatArray AS $value){
				$compatOperatingSystem=(cleanInteger(@$_POST['OperatingSystem_'.$value]))?@$_POST['OperatingSystem_'.$value]:NULL;
				$compatDistro=(cleanInteger(@$_POST['distro_'.$value]))?@$_POST['distro_'.$value]:NULL;
				$compatMustBuild=@$_POST['mustBuild_'.$value];
				$compatComments=cleanString(@$_POST['comments_'.$value]);
				if(isset($_POST['deleteCompatibility_'.$value])){
					$deletePackageSourceCompat='DELETE FROM Package_Source_Compat WHERE PK_Package_Source_Compat=?';
					$dbADO->Execute($deletePackageSourceCompat,$value);
				}else{
					$updatePackageSourceCompat='
						UPDATE Package_Source_Compat SET
							FK_OperatingSystem=?,
							FK_Distro=?,
							MustBuildFromSource=?,
							Comments=?
						WHERE PK_Package_Source_Compat=?';
					$dbADO->Execute($updatePackageSourceCompat,array($compatOperatingSystem,$compatDistro,$compatMustBuild,$compatComments,$value));
				}
			}
			
			foreach($displayedSourcesArray AS $value){
				$sourceName=@$_POST['source_'.$value];
				$sourceRepositorySource=(@$_POST['repositorySource_'.$value]!='0')?@$_POST['repositorySource_'.$value]:NULL;
				$sourceRepository=@$_POST['repository_'.$value];
				$sourceVersion=@$_POST['version_'.$value];
				$sourceParms=@$_POST['parms_'.$value];
				$updatePackageSource='
					UPDATE Package_Source SET
						Name=?,
						FK_RepositorySource=?,
						Repository=?,
						Version=?,
						Parms=?
					WHERE PK_Package_Source=?';
				$dbADO->Execute($updatePackageSource,array($sourceName,$sourceRepositorySource,$sourceRepository,$sourceVersion,$sourceParms,$value));
				
				if(isset($_POST['deleteSource_'.$value])){
					$deletePackageSourceCompat='DELETE FROM Package_Source_Compat WHERE FK_Package_Source=?';
					$dbADO->Execute($deletePackageSourceCompat,$value);

					
					$deletePackageSource='DELETE FROm Package_Source WHERE PK_Package_Source=?';
					$dbADO->Execute($deletePackageSource,$value);
				}
				
				// add compatibility
				if(isset($_POST['addCompatibility_'.$value])){
					$insertPackageSourceCompat='INSERT INTO Package_Source_Compat (FK_Package_Source) VALUES (?)';
					$dbADO->Execute($insertPackageSourceCompat,$value);
				}
			}
			
			//directory added to package
			if ($FK_Directory!=0) {
				$query = "INSERT INTO Package_Directory (FK_Package , FK_Directory)
						VALUES ($PK_Package,$FK_Directory)";
				$dbADO->Execute($query);
			}
			
			// update Package_Directory
			$displayedPackageDirectoryArray=explode(',',$_POST['displayedPackageDirectory']);
			foreach($displayedPackageDirectoryArray as $elem){
				$pdOperatingSystem=(cleanInteger(@$_POST['packageDirectoryOS_'.$elem]))?cleanInteger(@$_POST['packageDirectoryOS_'.$elem]):NULL;
				$pdDistro=(cleanInteger(@$_POST['packageDirectoryDistro_'.$elem]))?cleanInteger(@$_POST['packageDirectoryDistro_'.$elem]):NULL;
				$pdPath=cleanString(@$_POST['packageDirectoryPath_'.$elem]);
				$pdInputPath=(@$_POST['packageDirectoryInputPath_'.$elem]!='')?cleanString(@$_POST['packageDirectoryInputPath_'.$elem]):NULL;
				$pdFlipSource=(@$_POST['packageDirectoryFlipSource_'.$elem]=='1')?1:0;
				$updatePackageDirectory='UPDATE Package_Directory SET FK_OperatingSystem=?, FK_Distro=?, Path=?, InputPath=?, FlipSource=? WHERE PK_Package_Directory=?';
				$dbADO->Execute($updatePackageDirectory,array($pdOperatingSystem,$pdDistro,$pdPath,$pdInputPath,$pdFlipSource,$elem));
			}

			// update  Package_Directory_Files
			$displayedPackageDirectoryFilesArray=explode(',',$_POST['displayedPackageDirectoryFiles']);
			foreach($displayedPackageDirectoryFilesArray as $elem){
				$pdfOS=(cleanInteger(@$_POST['pdfOS_'.$elem]))?cleanInteger(@$_POST['pdfOS_'.$elem]):NULL;
				$pdfDistro=(cleanInteger(@$_POST['pdfDistro_'.$elem]))?cleanInteger(@$_POST['pdfDistro_'.$elem]):NULL;
				$pdfFile=cleanString(@$_POST['pdfFile_'.$elem]);
				$pdfMakeCommand=cleanString(@$_POST['pdfMakeCommand_'.$elem]);
				$updatePackageDirectoryFile='UPDATE Package_Directory_File SET FK_OperatingSystem=?, FK_Distro=?, File=?, MakeCommand=? WHERE PK_Package_Directory_File=?';
				$dbADO->Execute($updatePackageDirectoryFile,array($pdfOS,$pdfDistro,$pdfFile,$pdfMakeCommand,$elem));
				
			}
			$out.="
			<script>
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();";
			if(isset($_POST['saveClose']))
				$out.="self.close();";
			else
				$out.="self.location='index.php?section=addPackageToMasterDevice&from=".$from."&deviceID=$deviceID&PK_Package=$PK_Package';";
			$out.="
			</script>
			";			
		} else {
			$isSourceCode = isset($_REQUEST['isSourceCode'])?cleanInteger($_REQUEST['isSourceCode']):0;
			$description=$_POST['Description'];
			$FK_Package_Sourcecode=($isSourceCode==0)?(int)@$_POST['SourceCode']:NULL;
			$FK_PackageType=(@$_POST['PackageType'])?(int)@$_POST['PackageType']:NULL;
						
			$insertPackage='INSERT INTO Package (Description, FK_Package_Sourcecode, IsSource, FK_PackageType) VALUES (?,?,?,?)';
			$dbADO->Execute($insertPackage,array($description,$FK_Package_Sourcecode,$isSourceCode,$FK_PackageType));
			$PK_Package=$dbADO->Insert_ID();
			
			if($deviceID!=0){		
				$updateDeviceTemplate='UPDATE DeviceTemplate SET FK_Package=? WHERE PK_DeviceTemplate=?';
				$dbADO->Execute($updateDeviceTemplate,array($PK_Package,$deviceID));
			}
			
			header("Location: index.php?section=addPackageToMasterDevice&from=$from&deviceID=$deviceID&PK_Package=$PK_Package");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>