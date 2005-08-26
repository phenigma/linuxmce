<?
function ir($output)
{
	global $dbPlutoAdminServer,$dbPlutoAdminUser,$dbPlutoAdminPass,$dbPlutoAdminDatabase;
	include_once('include/plutoAdminUtils.inc.php');

	$publicADO = &ADONewConnection('mysql');
	$publicADO->NConnect($dbPlutoAdminServer,urlencode($dbPlutoAdminUser),urlencode($dbPlutoAdminPass),urlencode($dbPlutoAdminDatabase));


	/* @var $dbADO ADOConnection */
	global $PlutoHomeHost;
	$userID=(int)@$_SESSION['userID'];

	$out='
	<span class="normaltext" style="padding:20px;">
	<p>Find infrared codes in Pronto<sup>1</sup> format, or in Pluto\'s GSD format for devices with an RS232, RS485, USB,
	or Ethernet control.  We have <b>over 100,000 codes</b> for devices from <b>more than 1,000 manufacturers</b>.  If you find errors, please <a href="http://plutohome.com/index.php?section=support">contact us</a>.</p></span>';

	$selectedManufacturer = (int)@$_REQUEST['manufacturers'];
	$selectedDeviceCateg= (int)@$_REQUEST['deviceSelected'];
	
	if($selectedManufacturer==0 && $selectedDeviceCateg==0)
	{
		$out .= '<span class="title" style="padding:20px;"><p>Choose the manufacturer of your device and the category to see what models we have, or to add a new model.</p>
	</span>';
	}else{
		$sql1='SELECT COUNT(PK_InfraredGroup) As NumGroups,min(DeviceCategory.Description) as DeviceCategory,min(Manufacturer.Description) as Manufacturer
		FROM InfraredGroup
		JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
		JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		WHERE FK_Manufacturer=? and FK_DeviceCategory=?';
		$resGroup=$publicADO->Execute($sql1,array($selectedManufacturer,$selectedDeviceCateg));
		
		$addButton='<input type="button" class="button" name="addDeviceTemplate" value="Add model" onClick="javascript:addModel();"/>';
		
		if($resGroup && ($rowGroup=$resGroup->FetchRow()) && $rowGroup['NumGroups']>0)
		{
			$sql2='SELECT COUNT(PK_InfraredGroup_Command) As NumCodes
			FROM InfraredGroup_Command
			JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup
			WHERE FK_Manufacturer=? and FK_DeviceCategory=?';
			$resCodes=$publicADO->Execute($sql2,array($selectedManufacturer,$selectedDeviceCateg));
			$rowCodes=$resCodes->FetchRow();
			
			$out .= '<span class="title"><p><b>Good news!</b></span>' .
			'<span class="normaltext" style="padding-left:20px;">  We have <b>' .
			$rowCodes['NumCodes'] . '</b> codes in <b>' . 
			$rowGroup['NumGroups'] . '</b> groups already in the database for <b>' . $rowGroup['DeviceCategory'] . 
			'</b> by <b>' . $rowGroup['Manufacturer'] . '</b>.  Even if your exact model isn\'t listed below, you probably will
			not need to learn any codes because normally ' . $rowGroup['Manufacturer'] . ' uses the same groups of codes for all their '
			. $rowGroup['DeviceCategory'] . '.  If your model is shown, select it from the list and click \'Show Model\'.  If not,
			click </span>'.$addButton.'<span class="normaltext"> to add your exact model and our wizard will help you confirm the correct infrared group, or codeset, and 
			add any new codes that aren\'t in the database yet.</span>';
		}
		else
		{
			$sql2 = 'SELECT Description FROM Manufacturer WHERE PK_Manufacturer=?';
			$sql3 = 'SELECT Description FROM DeviceCategory WHERE PK_DeviceCategory=?';
            $resMfr=$publicADO->Execute($sql2,array($selectedManufacturer));
			$rowMfr=$resMfr->FetchRow();
            $resCat=$publicADO->Execute($sql3,array($selectedDeviceCateg));
			$rowCat=$resCat->FetchRow();
			
			
									
			$out .= '<span class="title"> <b>Sorry</b></span> <span class="normaltext">There aren\'t yet any codes in the database for <b>' . $rowCat['Description'] . '</b> by <b>' . 
			$rowMfr['Description'] . '</b>.  This is a free, public database.  Please take a moment to supply information and codes 
			for your model by clicking '.$addButton.'</span>';
		}
		
		
	}
	

/*
	if($userID==0){
		$out='
	<h3>Devices & IR Codes <a href="'.$PlutoHomeHost.'index.php?section=login">[ login required ]</a></h3>
	<p class="normaltext">Because you can add new devices and infrared codes, you must <a href="'.$PlutoHomeHost.'index.php?section=register">register</a> or <a href="'.$PlutoHomeHost.'index.php?section=login">login</a> to use this section.  That way when you add a device, or update infrared codes, you become the "owner" of that device.  As the owner, you can make any changes to your device, or overwrite your infrared codes.  But if another user makes a change to your device, those changes are isolated in a special place and given a "Batch number".  As the owner of the device, you will be sent an email that another user wants to make changes to your device.  That email will include the proposed changes, and you can click a link in the email to accept or reject the changes.<br><br>
	<p class="normaltext">This system allows the database to grow dynamically, and lets everyone build upon what others have done, but still provides a control mechanism so that other users cannot mess up what you have done.<br><br>
	<p class="normaltext">If you do not want to be notified when other users change your device, you can freeze it (meaning no changes are allowed), or make it anonymous (meaning anyone can change it).';
	}else{
		$firstColLinks='';

		$out=pickDeviceTemplate($GLOBALS['rootAVEquipment'],1,0,0,0,1,'ir',$firstColLinks,$publicADO,1);
	}
*/
	
	$firstColLinks='';
	$out.=pickDeviceTemplate($GLOBALS['rootAVEquipment'],1,0,0,0,0,'ir',$firstColLinks,$publicADO,1);
	$out.='<span class="normaltext" style="padding:20px;"><p>Note: All infrared codes are provided by users and Pluto assumes no responsibility for the codes.  <br>
	<sup>1</sup> <a href="http://www.pronto.philips.com/">Pronto</a> is a product and trademark of <a href="http://www.philips.com/index.html">Philips</a></p></span>';
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>
