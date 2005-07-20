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

	// for Aaron: replace the text below with your comments
	$out='
	<br><span class="normaltext" style="padding:20px;">

		Instructions: will be added soon.
	
	</span><br><br>';
	
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
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>