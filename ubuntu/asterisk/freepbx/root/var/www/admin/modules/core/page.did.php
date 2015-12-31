<?php /* $Id: page.did.php 4905 2007-08-21 04:41:29Z p_lindheimer $ */
//Copyright (C) 2004 Coalescent Systems Inc. (info@coalescentsystems.ca)
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
	
$action = isset($_REQUEST['action'])?$_REQUEST['action']:'';
$extdisplay= isset($_REQUEST['extdisplay'])?$_REQUEST['extdisplay']:'';
$dispnum = 'did'; //used for switch on config.php
$account = isset($_REQUEST['account'])?$_REQUEST['account']:'';
$goto = isset($_REQUEST['goto0'])?$_REQUEST['goto0']:'';
$ringing = isset($_REQUEST['ringing'])?$_REQUEST['ringing']:'';
$description = isset($_REQUEST['description'])?$_REQUEST['description']:'';

//update db if submiting form
switch ($action) {
	case 'addIncoming':
		//create variables from request
		extract($_REQUEST);
		//add details to teh 'incoming' table
		if (core_did_add($_REQUEST)) {
			needreload();
			redirect_standard();
		}
	break;
	case 'delIncoming':
		$extarray=explode('/',$extdisplay,3);
		core_did_del($extarray[0],$extarray[1],$extarray[2]);
		needreload();
		redirect_standard();
	break;
	case 'edtIncoming':
		$extarray=explode('/',$extdisplay,3);
		if (core_did_edit($extarray[0],$extarray[1],$extarray[2],$_REQUEST)) {
			needreload();
			redirect_standard('extdisplay');
		}
	break;
}

?>
</div>

<div class="rnav">
<ul>
	<li><a <?php echo ($extdisplay=='' ? 'class="current"':'') ?> href="config.php?display=<?php echo urlencode($dispnum)?>"><?php echo _("Add Incoming Route")?></a></li>
<?php 
//get unique incoming routes
$inroutes = core_did_list('description');
if (isset($inroutes)) {
	foreach ($inroutes as $inroute) {
		$displaydid = ( empty($inroute['extension'])? _("any DID") : $inroute['extension'] );
 		$displaycid = ( empty($inroute['cidnum'])? _("any CID") : $inroute['cidnum'] );
		$zapchan = ( strlen($inroute['channel'])? "Zaptel Channel {$inroute['channel']}" : "" );
		$desc = ( empty($inroute['description'])? "" : $inroute['description']."<br />" );
		if ($zapchan != "") 
			echo "\t<li><a ".($extdisplay==$inroute['extension']."/".$inroute['cidnum']."/".$inroute['channel'] ? 'class="current"':'')." href=\"config.php?display=".urlencode($dispnum)."&amp;extdisplay=".urlencode($inroute['extension'])."/".urlencode($inroute['cidnum'])."/".urlencode($inroute['channel'])."\">{$desc} {$zapchan} </a></li>\n";
		else
			echo "\t<li><a ".($extdisplay==$inroute['extension']."/".$inroute['cidnum']."/".$inroute['channel'] ? 'class="current"':'')." href=\"config.php?display=".urlencode($dispnum)."&amp;extdisplay=".urlencode($inroute['extension'])."/".urlencode($inroute['cidnum'])."/".urlencode($inroute['channel'])."\">{$desc} {$displaydid} / {$displaycid} </a></li>\n";
	}
}
?>
</ul>
</div>

<div class="content">
<?php 
	
	if ($action == 'delIncoming') {
		echo '<br><h3>Route '.$extdisplay.' '._("deleted").'!</h3><br><br><br><br><br><br><br><br>';
	} else {
		$delURL = $_SERVER['PHP_SELF'].'?'.$_SERVER['QUERY_STRING'].'&action=delIncoming';
?>
<?php if ($extdisplay) {	
	//create variables for the selected route's settings
	$extarray=explode('/',$extdisplay,3);
	$ininfo=core_did_get($extarray[0],$extarray[1],$extarray[2]);
	if (is_array($ininfo)) extract($ininfo);
?>
		<h2><?php echo _("Route")?>: <?php echo !empty($description)?$description:$extdisplay; ?></h2>
		<p><a href="<?php echo $delURL ?>"><?php echo _("Delete Route")?> <?php echo !empty($description)?$description:$extdisplay ?></a></p>
<?php } else { ?>
		<h2><?php echo _("Add Incoming Route")?></h2>
<?php } ?>
		<form name="editGRP" action="<?php $_SERVER['PHP_SELF'] ?>" method="post" onsubmit="return editGRP_onsubmit();">
		<input type="hidden" name="display" value="<?php echo $dispnum?>">
		<input type="hidden" name="action" value="<?php echo ($extdisplay ? 'edtIncoming' : 'addIncoming') ?>">
		<input type="hidden" name="extdisplay" value="<?php echo $extdisplay ?>">
		<table>
		<tr><td colspan="2"><h5><?php echo ($extdisplay ? _('Edit Incoming Route') : _('Add Incoming Route')) ?><hr></h5></td></tr>
		<tr>
			<td><a href="#" class="info"><?php echo _("Description")?><span><?php echo _('Provide a meaningful description of what this incoming route is')?></span></a>:</td>
			<td><input type="text" name="description" value="<?php echo htmlspecialchars(isset($description)?$description:''); ?>"></td>
		</tr>
		<tr>
			<td><a href="#" class="info"><?php echo _("DID Number")?><span><?php echo _('Define the expected DID Number if your trunk passes DID on incoming calls. <br><br>Leave this blank to match calls with any or no DID info.<br><br>You can also use a pattern match (eg _2[345]X) to match a range of numbers')?></span></a>:</td>
			<td><input type="text" name="extension" value="<?php echo htmlspecialchars(isset($extension)?$extension:''); ?>"></td>
		</tr>
		<tr>
			<td><a href="#" class="info"><?php echo _("Caller ID Number")?><span><?php echo _('Define the Caller ID Number to be matched on incoming calls.<br><br>Leave this field blank to match any or no CID info.')?></span></a>:</td>
			<td><input type="text" name="cidnum" value="<?php echo htmlspecialchars(isset($cidnum)?$cidnum:'') ?>"></td>
		</tr>
		<tr><td><h4><?php echo _("OR"); ?></h4></td></tr>
		<tr>
			<td><a href="#" class="info"><?php echo _("Zaptel Channel")?><span><?php echo _('Match calls that come in on this specific Zaptel channel number. zapata.conf must have "context=from-zaptel" rather than context="from-pstn" to use this feature')?></span></a>:</td>
			<td><input type="text" name="channel" value="<?php echo htmlspecialchars(isset($channel)?$channel:''); ?>"></td>
		</tr>
		<tr>
			<td><br></td>
		</tr>
		<tr><td colspan="2"><h5><?php echo _("Fax Handling")?><hr></h5></td></tr>
		<tr>
			<td>
				<a class="info" href="#"><?php echo _("Fax Extension")?><span><?php echo _("Select 'system' to have the system receive and email faxes.<br><br>The FreePBX default is defined in General Settings.")?></span></a>:
			</td>
			<td>&nbsp;
				<select name="faxexten">
<?php 
// Cleaning up warnings. I should do this a better way.
if (!isset($faxexten))
	$faxexten = null;
if (!isset($faxemail))
	$faxemail = null;
if (!isset($answer))
	$answer = '0';
if (!isset($alertinfo))
	$alertinfo = 0;
if (!isset($mohclass))
	$mohclass = 'default';
if (!isset($grppre))
	$grppre = "";
?>
					<option value="default" <?php  echo ($faxexten == 'default' ? 'SELECTED' : '')?>><?php echo _("FreePBX default")?>
					<option value="disabled" <?php  echo ($faxexten == 'disabled' ? 'SELECTED' : '')?>><?php echo _("disabled")?>
					<option value="system" <?php  echo ($faxexten == 'system' ? 'SELECTED' : '')?>><?php echo _("system")?>
			<?php 
				//get unique devices
				$devices = core_devices_list();
				if (isset($devices)) {
					foreach ($devices as $device) {
						echo '<option value="'.$device[0].'" '.($faxexten == $device[0] ? 'SELECTED' : '').'>'.$device[1].' &lt;'.$device[0].'&gt;';
					}
				}
			?>	
				</select>
			</td>
		</tr>
		<tr>
			<td>
				<a class="info" href="#"><?php echo _("Fax Email")?><span><?php echo _("Email address is used if 'system' has been chosen for the fax extension above.<br><br>Leave this blank to use the FreePBX default in General Settings.")?></span></a>:
			</td>
			<td>
				<input type="text" size="20" name="faxemail" value="<?php echo htmlspecialchars($faxemail)?>"/>
			</td>
		</tr>
		<tr>
			<td><a href="#" class="info"><?php echo _("Fax Detection Type")?><span><?php echo _('Selecting Zaptel or NVFax will immediately answer the call and play ringing tones to the caller for the number of seconds in Pause below. Use NVFax on SIP or IAX trunks.')?></span></a>:</td>
			<td>&nbsp;
				<select name="answer">
					<option value="0" <?php  echo ($answer == '0' ? 'SELECTED' : '')?>><?php echo _("None")?>
					<option value="1" <?php  echo ($answer == '1' ? 'SELECTED' : '')?>><?php echo _("Zaptel")?>
					<option value="2" <?php  echo ($answer == '2' ? 'SELECTED' : '')?>><?php echo _("NVFax")?>
				</select>
			</td>
		</tr>
		<tr>
			<td><a href="#" class="info"><?php echo _("Pause after answer")?><span><?php echo _('The number of seconds we should wait after performing an Immediate Answer. The primary purpose of this is to pause and listen for a fax tone before allowing the call to proceed.')?></span></a>:</td>
			<td><input type="text" name="wait" size="3" value="<?php echo isset($wait)?$wait:'' ?>"></td>
		</tr>
		<tr>
			<td><br></td>
		</tr>
<?php
if (!isset($privacyman))
	$privacyman = '0';
?>
		<tr><td colspan="2"><h5><?php echo _("Privacy")?><hr></h5></td></tr>
		<tr>
			<td><a href="#" class="info"><?php echo _("Privacy Manager")?><span><?php echo _('If no Caller ID is sent, Privacy Manager will asks the caller to enter their 10 digit phone number. The caller is given 3 attempts.')?></span></a>:</td>
			<td>
				<select name="privacyman">
					<option value="0" <?php  echo ($privacyman == '0' ? 'SELECTED' : '')?>><?php echo _("No")?>
					<option value="1" <?php  echo ($privacyman == '1' ? 'SELECTED' : '')?>><?php echo _("Yes")?>
				</select>
			</td>
		</tr>
		<tr>
			<td><br></td>
		</tr>		
		
		<tr><td colspan="2"><h5><?php echo _("Options")?><hr></h5></td></tr>



		<tr>
			<td><a href="#" class="info"><?php echo _("Alert Info")?><span><?php echo _('ALERT_INFO can be used for distinctive ring with SIP devices.')?></span></a>:</td>
			<td><input type="text" name="alertinfo" size="10" value="<?php echo ($alertinfo)?$alertinfo:'' ?>"></td>
		</tr>
		<tr>
			<td><a href="#" class="info"><?php echo _("CID name prefix")?>:<span><?php echo _('You can optionally prefix the Caller ID name. ie: If you prefix with "Sales:", a call from John Doe would display as "Sales:John Doe" on the extensions that ring.')?></span></a>:</td>
			<td><input type="text" name="grppre" size="10" value="<?php echo ($grppre)?$grppre:'' ?>"></td>
		</tr>
<?php   if (function_exists('music_list')) { ?>
		<tr>
			<td><a href="#" class="info"><?php echo _("Music On Hold?")?><span><?php echo _("Set the MoH class that will be used for calls that come in on this route. For example, choose a type appropriate for routes coming in from a country which may have announcements in their language.")?></span></a></td>
			<td>
				&nbsp;&nbsp;<select name="mohclass"/>
				<?php
					$tresults = music_list($amp_conf['ASTVARLIBDIR']."/mohmp3");
					$cur = (isset($mohclass) && $mohclass != "" ? $mohclass : 'default');
					echo '<option value="none">'._("No Music")."</option>";
					if (isset($tresults[0])) {
						foreach ($tresults as $tresult) {
							echo '<option value="'.$tresult.'"'.($tresult == $cur ? ' SELECTED' : '').'>'.$tresult."</option>\n";
						}
					}
				?>		
				</select>		
			</td>
		</tr>
<?php } ?>
		<tr>
			<td><a href="#" class="info"><?php echo _("Signal RINGING")?><span><?php echo _('Some devices or providers require RINGING to be sent before ANSWER. You\'ll notice this happening if you can send calls directly to a phone, but if you send it to an IVR, it won\'t connect the call.')?></span></a>:</td>
			<td><input type="checkbox" name="ringing" value="CHECKED" <?php echo $ringing ?> /></td>
		</tr>


<?php
	// implementation of module hook
	// object was initialized in config.php
	echo $module_hook->hookHtml;
?>

		<tr>
			<td><br></td>
		</tr>		
		
		<tr><td colspan="2"><h5><?php echo _("Set Destination")?><hr></h5></td></tr>
		
<?php 
//draw goto selects
echo drawselects(isset($destination)?$destination:null,0);
?>
		
		<tr>
		<td colspan="2"><br><h6>
			<input name="Submit" type="submit" value="<?php echo _("Submit")?>">
		</h6></td>		
		
		</tr>
		</table>
<script language="javascript">
<!--

var theForm = document.editGRP;

theForm.extension.focus();

function editGRP_onsubmit() {
	var msgInvalidDIDNumb = "<?php echo _('Please enter a valid DID Number'); ?>";
	var msgInvalidCIDNum = "<?php echo _('Please enter a valid Caller ID Number'); ?>";
	var msgCidDidWithZaptel = "<?php echo _('DID number and CID number MUST be blank when used with zaptel channel routing'); ?>";
	var msgInvalidFaxEmail = "<?php echo _('Please enter a valid Fax Email or leave it empty to use the default'); ?>";
	var msgInvalidPause = "<?php echo _('Please enter a valid number for Pause after answer'); ?>";
	var msgConfirmDIDCIDBlank = "<?php echo _('Leaving the DID Number AND the Caller ID Number empty will match all incoming calls received not routed using any other defined Incoming Route.\n\nAre you sure?'); ?>";
	var msgConfirmDIDNonStd = "<?php echo _('DID information is normally just an incoming telephone number or for advanced users, a valid Asterisk Dial Pattern\n\nYou have entered a non standard DID pattern.\n\nAre you sure this is correct?'); ?>";
	var msgConfirmDIDNoSlash = "<?php echo _('A Slash (\'/\') is never a valid DID. Please remove it and try again'); ?>";
	var msgInvalidGrpPrefix = "<?php echo _('Invalid Caller ID prefix.'); ?>";
	
	setDestinations(theForm,1);
	
	defaultEmptyOK = true;
	if (!isDialpattern(theForm.extension.value)) {
		// warn the user that DID is normally numbers
		if (!confirm(msgConfirmDIDNonStd))
			return false;
	}
	
	if (isInside(theForm.extension.value, "/")) {
		warnInvalid(theForm.extension, msgConfirmDIDNoSlash);
		return false;
	}
	if (!isDialpattern(theForm.cidnum.value))
		return warnInvalid(theForm.cidnum, msgInvalidCIDNum);
	
	if (!isEmail(theForm.faxemail.value))
		return warnInvalid(theForm.faxemail, msgInvalidFaxEmail);
	
	if (!isInteger(theForm.wait.value))
		return warnInvalid(theForm.wait, msgInvalidPause);
	
	if (!validateDestinations(theForm,1,true))
		return false;
	
	if (theForm.extension.value != ""  && theForm.channel.value != "" ) {
		return warnInvalid(theForm.extension, msgCidDidWithZaptel);
	}
	if (theForm.cidnum.value != ""  && theForm.channel.value != "" ) {
		return warnInvalid(theForm.cidnum, msgCidDidWithZaptel);
	}
	// warning about 'any DID / any CID'
	if (theForm.extension.value == "" && theForm.cidnum.value == "" && theForm.channel.value == "" ) {
		if (!confirm(msgConfirmDIDCIDBlank))
			return false;
	}
	defaultEmptyOK = true;
	if (!isCallerID(theForm.grppre.value))
		return warnInvalid(theForm.grppre, msgInvalidGrpPrefix);
	
	return true;
}

//-->
</script>
		</form>
<?php 		
	} //end if action == delGRP
	

?>





