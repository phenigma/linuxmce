<?
if($action=='form'){
	// display form
	$out = '
		   <form action="index.php" method="POST" name="wizard">
			<input type="hidden" name="section" value="wizard">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">	
	      		<table align="center">
					<tr>
						<td align="left" class="insidetable"><img src="images/titles/installation_wizzard.gif"></td>
	      				<td align="right" class="insidetable"><a href="index.php?section=wizard&step=2">&lt;&lt; Previous</a> <a href="#" onClick="javascript:document.wizard.submit();">Next &gt;&gt;</a></td>
	      			</tr>
	      			<tr>
	      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 3 of 8: '.((!isset($_SESSION['installationDescription']))?'New Installation':'Update Installation').'</b></td>
	      			</tr>
					<tr>
						<td colspan="2" class="insidetable2">Please pick a name for your new installation, like "123 Main Street" or "Beach House".  If you have multiple homes, you should create an installation for each home and then "join" them together later.</td>
					</tr>
	      	      	<tr>
	      				<td align="right" class="normaltext"><B>Name</B></td>
						<td align="left" class="insidetable2"><input type="text" name="Description" value="'.@stripslashes($_SESSION['installationDescription']).'"> <input type="submit" name="submitBtn" value="Next"></td>
	      			</tr>
	      		</table>
			</form>
		 	<script>
 				var frmvalidator = new formValidator("wizard");
				frmvalidator.addValidation("Description","req","Please enter a description.");
			</script>
	      	';

}else{
	// process form
	$description=@$_POST['Description'];

	if(isset($_POST['submitBtn']) || $action=='add'){
		if($installationID==0){
			$randNumber=rand(1000,9999);
			$insertInstallation='INSERT INTO Installation (Description, ActivationCode) VALUES (?,?)';
			$dbADO->Execute($insertInstallation,array($description,$randNumber));
			$installationID=$dbADO->Insert_ID();
			$_SESSION['installationID']=$installationID;
			$_SESSION['installationDescription']=$description;
			$_SESSION['ActivationCode']=$randNumber;

			$insertInstallationUsers='
							INSERT INTO Installation_Users 
								(FK_Installation, FK_Users, userCanModifyInstallation)
							VALUES (?,?,?)';
			$dbADO->Execute($insertInstallationUsers,array($installationID,$FK_Users,1));

			$updateUser='UPDATE Users SET FK_Installation_Main=? WHERE PK_Users=?';
			$dbADO->Execute($updateUser,array($installationID,$FK_Users));
		}else{
			$installationID=(isset($_SESSION['installationID']))?$_SESSION['installationID']:0;
			$updateInstallation='UPDATE Installation SET Description=? WHERE PK_Installation=?';
			$dbADO->Execute($updateInstallation,array($description,$installationID));
			$_SESSION['installationDescription']=$description;
		}
		header("Location: index.php?section=wizard&step=4");
		exit();
	}
}
?>
