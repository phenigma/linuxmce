<?
function uploadFloorplan($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$page=(isset($_REQUEST['fpID']))?cleanInteger($_REQUEST['fpID']):0;
	$type=cleanInteger(@$_REQUEST['type']);
	$strError=(isset($_GET['error']))?$_GET['error']:'';
	
	$installationID = (int)@$_SESSION['installationID'];
	$path=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/floorplans/inst'.$installationID;
	$floorplanImage='floorplans/image_not_found.gif';
	if(file_exists($path.'/'.$page.'.jpg'))
		$floorplanImage='floorplans/inst'.$installationID.'/'.$page.'.jpg';
	elseif(file_exists($path.'/'.$page.'.png'))
		$floorplanImage='floorplans/inst'.$installationID.'/'.$page.'.png';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$out='';
	
	$queryFloorplan='SELECT Description FROM Floorplan WHERE FK_Installation=? AND Page=?';
	$resFloorplan=$dbADO->Execute($queryFloorplan,array($installationID,$page));
	if($resFloorplan->RecordCount()==0){
		header("Location: index.php?section=uploadFloorplan&error=Invalid floorplan specified");		
		exit();
	}
	$rowFloorplan=$resFloorplan->FetchRow();
	$oldDescription=$rowFloorplan['Description'];
	if($action=='form'){
		$out='
			<form action="index.php" method="POST" name="uploadFloorplan" enctype="multipart/form-data">
			<input type="hidden" name="section" value="uploadFloorplan">
			<input type="hidden" name="action" value="add">		
			<input type="hidden" name="fpID" value="'.$page.'">		
			<input type="hidden" name="type" value="'.$type.'">		
		
			<table bgcolor="#EEEEEE" cellpadding="0" cellspacing="0" border="1" align="center">
				<tr>
					<td>
					<table cellpadding="20">
						<tr>
							<td>&nbsp;&nbsp;&nbsp;</td>
							<td>';

								if($strError != ''){ 
									$out.='<p class="normalBold">'.$strError.'</p>';
								} 
								if($page != 0)
								{ 
									$out.='<img src="'.$floorplanImage.'"  border="1"><br /><br />'; 
								}
							$out.='	
								<table>
									<tr>
										<td class="normal">Description&nbsp;&nbsp;&nbsp;</td>
										<td><input type="text" name="txtDescription" value="'.@$oldDescription.'"></td>
									</tr>
									<tr>
										<td class="normal">
											Choose an file:<br />&nbsp; &nbsp; (must be PNG or JPG)
										</td>
										<td><input type="file" name="fileImage"></td>
									</tr>
									<tr>
										<td colspan="2" align="center"><br />
											<input type="submit" value="'.(($page==0)?'Add':'Update').' Floorplan">
											<input type="button" onclick="JavaScript:self.location=\'index.php?section=floorplanWizard&page='.$page.'&type='.$type.'\';" value="Cancel">&nbsp; &nbsp;
										</td>
									</tr>
								</table>
							</td>
						</tr>
					</table>
					</td>
				</tr>
			</table>
		</form>
		';
	}else{
		
		$newDescription=cleanString($_POST['txtDescription']);
		$path=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/floorplans/inst'.$installationID;
		
		switch($_FILES['fileImage']['type']){
			case 'image/x-png':
			case 'image/png': $extension = 'png'; break;
			case 'image/pjpeg':
			case 'image/jpg':
			case 'image/jpeg': $extension = 'jpg'; break;
			default:
				$invalidType=true;
			break;
		}
		
		if(isset($invalidType)){
			header("Location: index.php?section=uploadFloorplan&error=Invalid file type.");
			exit();
		}
		
		if(!file_exists($path) && !@mkdir($path)){
			header("Location: index.php?section=uploadFloorplan&error=Cannot create directory.");
			exit();
		}
		
		if($page==0){
			$selectPage='SELECT (MAX(Page)+1) AS newPage FROM Floorplan WHERE FK_Installation=?';
			$res=$dbADO->Execute($selectPage,$installationID);
			$row=$res->FetchRow();
			$newPicName=$row['newPage'].'.'.$extension;
		}else
			$newPicName=$page.'.'.$extension;
		
		if(!move_uploaded_file($_FILES['fileImage']['tmp_name'],$path.'/'.$newPicName)){
			header("Location: index.php?section=uploadFloorplan&error=Upload failed. Check the rights for $path");
			exit();
		}else{
			$otherExtension=($extension=='jpg')?'png':'jpg';
			if(file_exists($path.'/'.$page.'.'.$otherExtension)){
				unlink($path.'/'.$page.'.'.$otherExtension);
			}
		}
		
		if($page==0){
			$insertFloorplan='INSERT INTO Floorplan (FK_Installation, Page, Description) VALUES (?,?,?)';
			$dbADO->Execute($insertFloorplan,array($installationID,$row['newPage'],$newDescription));
			$page=$row['newPage'];
		}
		header("Location: index.php?section=floorplanWizard&page=".$page."&type=".$type);
	}
	$output->setNavigationMenu(array("Floorplans Wizard"=>'index.php?section=floorplanWizard&page='.$page.'&type='.$type));	
	
	$output->setHelpSrc('/support/index.php?section=document&docID=35');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>