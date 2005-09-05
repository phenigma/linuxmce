<?
	$dtID=$_REQUEST['dtID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	if($dtID==0){
		header('Location: index.php');
		exit();
	}

	$dtDataArray=getFieldsAsArray('DeviceTemplate','FK_DeviceCategory,DeviceCategory.Description AS Category,Manufacturer.Description AS Manuf,FK_Manufacturer,FK_DeviceCategory',$publicADO,'INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer WHERE PK_DeviceTemplate='.$dtID);
	if(count($dtDataArray)==0){
		header('Location: index.php');
		exit();
	}

	if($action=='form'){
		$output->setScriptInHead('<script src="javascripts/user.js" type="text/javascript" language="JavaScript"></script>');
		$out='
		<script>
		function CopyPlusSelect(copyText) {
		     if (window.clipboardData) { // IE send-to-clipboard method.
		          window.clipboardData.setData(\'Text\', copyText);
		          
		     } else if (window.netscape) {
		          // You have to sign the code to enable this or allow the action in about:config by changing user_pref("signed.applets.codebase_principal_support", true);
		          netscape.security.PrivilegeManager.enablePrivilege(\'UniversalXPConnect\');
		          
		          // Store support string in an object.
		          var str = Components.classes["@mozilla.org/supports-string;1"].createInstance(Components.interfaces.nsISupportsString);
		          if (!str) return false;
		          str.data=copyText;
		          
		          // Make transferable.
		          var trans = Components.classes["@mozilla.org/widget/transferable;1"].createInstance(Components.interfaces.nsITransferable);
		          if (!trans) return false;
		          
		          // Specify what datatypes we want to obtain, which is text in this case.
		          trans.addDataFlavor("text/unicode");
		          trans.setTransferData("text/unicode",str,copyText.length*2);
		          
		          var clipid=Components.interfaces.nsIClipboard;
		          var clip = Components.classes["@mozilla.org/widget/clipboard;1"].getService(clipid);
		          if (!clip) return false;
		          
		          clip.setData(trans,null,clipid.kGlobalClipboard);
		     }
		}
		</script>
		<br>
		<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step-1).'&deviceID='.$deviceID.'">&lt;&lt;</a></div>
		<B>Pick the Infrared Group that works for your device</B><br><br>
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
		';

		$rs=$publicADO->Execute('
			SELECT PK_InfraredGroup,Description
			FROM InfraredGroup
			WHERE FK_DeviceCategory=? AND FK_Manufacturer=?',array($dtDataArray['FK_DeviceCategory'][0],$dtDataArray['FK_Manufacturer'][0]));
		if($rs->RecordCount()>0){
			$out.='<p class="normaltext">Chances are you will <B>not need to learn or enter i/r codes</B> because we have '.$rs->RecordCount().' 
				Groups (codesets) for manufacturer <B>'.$dtDataArray['Manuf'][0].'</B>, device category  <B>'.$dtDataArray['Category'][0].'</B>.  
				Likely one of them works your model too.  Here are a few common commands for each i/r group.  
				Click the ‘V’ button to view the code in a new window, or the ‘T’ button to test it using the above transmit device.  
				If you find an i/r group that works your device, click “this works”.  
				Then you’ll see all the codes for that group, and you can add more codes or replace certain codes that don’t work.<br>';
			if($rs->RecordCount()>1){
				$out.='More than 1 group may have codes that work.
				If so, click the Infrared Group to see and test all the codes in that group, and pick the group that most closely matches your device.
				If none of these codes work, please type in a description for a new infrared group and click Add i/r group.<br>';
			} else {
				$out.='If this infrared group doesn\'t work, please type in a description for a new infrared group and click Add i/r group.<br>';
			}
				
			$out.='<div align="center" class="normaltext">';
			
			$InfraredGroupsArray=array();
			while($row=$rs->FetchRow()){
				$InfraredGroupsArray[]=$row['PK_InfraredGroup'];
				//$out.='<a href="index.php?section=irCodes&dtID='.$dtID.'&infraredGroupID='.$row['PK_InfraredGroup'].'">'.$row['Description'].'</a><br>';
			}
			
			$out.=getIrGroup_CommandsMatrix($dtID,$InfraredGroupsArray,@$_SESSION['userID'],@$_SESSION['selectedCommMethod'],$publicADO,$deviceID);
			$out.='</div>';
			
		}else{
			$out.='<p class="normaltext">Sorry that we don’t already have codes for manufacturer <B>'.$dtDataArray['Manuf'][0].'</B>, device category  <B>'.$dtDataArray['Category'][0].'</B>.  Please take a moment to add them so other users can benefit from your efforts.  Thanks!';
		}
		
		
			$out.='
			<table class="normaltext" align="center">
			<tr>
				<td align="center">Type in a description for the infrared group and click Add i/r group: <input type="text" name="description" value=""></td>
			</tr>
			<tr>
				<td align="center"><input type="submit" class="button" name="next" value="Add i/r group"></td>
			</tr>
		</table>
		<p class="normaltext"><B>Note:</B> when adding a i/r new group, if you know the codeset as used in a universal remote control, please use that as the description.  Example: “Pronto Codeset – 9999”, or “OFA Codeset – 999”.  For GSD devices, an example would be: “Denon AVR/AVC protocol”.
		<br>
		</form>
		';
		if(session_name()=='Pluto-admin'){
			$out.='<iframe name="codeTester" src="" style="display:none;"></iframe>';
		}
	}elseif($action=='changeParent'){
		// change parent and redirect to page
		$controlledVia=$_POST['controlledVia'];
		if($deviceID>0){
			$publicADO->Execute('UPDATE Device SET FK_Device_ControlledVia=? WHERE PK_Device=?',array($controlledVia,$deviceID));
		}
		$msg='The parent of the device was updated.';
		header('Location: index.php?section=addModel&step=7&dtID='.$dtID.'&deviceID='.$deviceID.'&msg='.$msg);
		exit();
		
	}else{
		// process
	
		$description=stripslashes($_POST['description']);
		if($description!=''){
			$publicADO->Execute('INSERT INTO InfraredGroup (FK_DeviceCategory,FK_Manufacturer,Description) VALUES (?,?,?)',array($dtDataArray['FK_DeviceCategory'][0],$dtDataArray['FK_Manufacturer'][0],$description));
			$irgID=$publicADO->Insert_ID();
			
			$publicADO->Execute('UPDATE DeviceTemplate SET FK_InfraredGroup=? WHERE PK_Devicetemplate=?',array($irgID,$dtID));
			
			header('Location: index.php?section=irCodes&dtID='.$dtID.'&infraredGroupID='.$irgID.'&deviceID='.$deviceID);
			exit();
		}else{
			header('Location: index.php?section=addModel&step=7&dtID='.$dtID.'&deviceID='.$deviceID);
			exit();
		
		}
			
	}
?>