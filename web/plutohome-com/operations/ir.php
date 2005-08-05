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
	<p>Find infrared code for your device in Pronto(1) format, or in Pluto\'s GSD format for devices with an RS232, RS485, USB,	or Ethernet control.  If you find errors, please <a href="http://plutohome.com/index.php?section=support">contact us</a>.</p>
	<p><b>1.</b>  Choose the manufactuer of your device, and then choose the category.
	<br><b>2a.</b>  If your model is shown in the list, select it, and click \'Show Model\' to get the codes and device properties.
	<br><b>2b.</b>  If your model is not shown, look at the Infrared Groups list below the models, also known as codesets.  Normally manufacturers	use the same groups of infrared codes across all similar models.  Try some codes from each Infrared Group.  	If you find one that works with your model, please follow the instructions on that page to add your model to the database.</p>		<p>If there are no codes for your model, you will have to learn them using Pluto\'s I/R learning modules, 	or another device like the Philips Pronto that can learn codes in raw pronto format(1).  
	Once you do, please take a moment to add them to this site.</p>

	<p><b><u>To add codes:</u></b>
	<br><b>1.</b>  Select the manufacturer.  If your manufacturer is not in the list, type it in the box below the list and click "Add manufacturer".
	<br><b>2.</b>  Select the category that best describes your device.  If you need a new category, please <a href="http://plutohome.com/index.php?section=support">contact us</a>.  	We\'ll add it right away.
	<br><b>3.</b>  Type in the model below the model list and click "Add New model".</p>
	
	</span>';
	
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
	$out.='<span class="normaltext" style="padding:20px;"><p>Note: All infrared codes are provided by users and Pluto assumes no responsibility for the codes.  (1) <a href="http://www.pronto.philips.com/">Pronto</a> is a product and trademark of <a href="http://www.philips.com/index.html">Philips</a></p></span>';
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>