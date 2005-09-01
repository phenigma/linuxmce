<?
	// the document with instructions for option 1, I'll write C++ code
	$writeCodeDocument=1;
	
	// the document for virtual device, option 3
	$virtualDeviceDocument=1;
	
	$manufArray=getAssocArray('Manufacturer','PK_Manufacturer','Description',$publicADO,'WHERE PK_Manufacturer='.$mID,'ORDER BY Description ASC');
	$categArray=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$publicADO,'WHERE PK_DeviceCategory='.$dcID,'ORDER BY Description ASC');
	
	if($action=='form'){
		$out='
		<h3 align="center">Add model</h3><br>
		<p class="normaltext">
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="mID" value="'.$mID.'">
			<input type="hidden" name="dcID" value="'.$dcID.'">
			<input type="hidden" name="from" value="'.$from.'">
		
		
		<table class="normaltext" align="center">
			<tr>
				<td align="right"><B>Manufacturer</B></td>
				<td>'.$manufArray[$mID].'</td>
				<td align="right"><B>Device Category</B></td>
				<td>'.$categArray[$dcID].'</td>
			</tr>';
		if($from!='genericSerialDevices'){
			$out.='
			<tr>
				<td><B>What type of device will this be?</B></td>
				<td colspan="3">
					<input type="radio" name="opt" value="1" checked> I\'m going to write c++ code<br>
					<input type="radio" name="opt" value="2"> I\'m going to use GSD and Ruby<br>
					<input type="radio" name="opt" value="3"> This is a virtual device that is a child of a dce device, and does not require any code			
				</td>
			</tr>			

			
			';
		}
			$out.='
			<tr>
				<td><B>What is the name or model #:</B></td>
				<td colspan="3"><input type="text" name="description" value=""> <input type="submit" class="button" name="add" value="Add"></td>
			</tr>
		</table><br>
		</form>
		
		<script>
			var frmvalidator = new formValidator("addModel");
 			frmvalidator.addValidation("description","req","Please type the name of the model.");
		</script>		
		';
	}else{
		// process
		$opt=(int)$_POST['opt'];
		$description=stripslashes($_POST['description']);
		if($from=='genericSerialDevices'){
			$opt=2;
		}

		switch ($opt){
			case 1:
				$implementDCE=1;
				$commandLine=NULL;
				$redirectJS='
				<script>
					self.location=\'index.php?section=userHome\';
					window.open(\''.$PlutoSupportHost.'index.php?section=document&docID='.$writeCodeDocument.'\',\'_blank\',\'\')
					self.close();
				</script>
				';
			break;

			case 2:
				$implementDCE=1;
				$commandLine='Generic_Serial_Device';
				$addInstance=1;
				$addCommandGroup=1;
				if($from=='genericSerialDevices'){
					$redirectJS='
					<script>
						opener.location=\'index.php?section=genericSerialDevices&msg=The device was added.\';
						self.close();
					</script>
					';
				}else{
				$redirectJS='
				<script>
					self.location=\'index.php?section=genericSerialDevices&msg=The device was added.\';
				</script>
				';

				}
			break;
			
			case 3:
				$implementDCE=0;
				$commandLine=NULL;
				$redirectJS='
				<script>
					self.location=\'index.php?section=userHome\';
					window.open(\''.$PlutoSupportHost.'index.php?section=document&docID='.$virtualDeviceDocument.'\',\'_blank\',\'\')
				</script>
				';				
			break;
		}
		
		if($dcID!=0 && $mID!=0 && $description!=''){
			$publicADO->Execute('INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer,psc_user,ImplementsDCE,CommandLine) values(?,?,?,?,?,?)',array($description,$dcID,$mID,$userID,$implementDCE,$commandLine));
			$dtID=$publicADO->Insert_ID();
			
			if($dtID>0 && isset($addInstance)){
				// create the instance
				global $dbPlutoMainDatabase;
				$installationID=(int)$_SESSION['installationID'];
				$deviceID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$dtID.' -i '.$installationID);
			}
			
			$out=$redirectJS;				
				
		}else{
			header('Location: index.php?section=addModel&dcID='.$dcID.'&mID='.$mID.'&from='.$from.'&error=Please type the description.');
		}
	}
?>