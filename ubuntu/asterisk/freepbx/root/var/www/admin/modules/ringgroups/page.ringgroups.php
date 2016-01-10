<?php /* $Id: page.ringgroups.php 4233 2007-06-27 04:03:12Z p_lindheimer $ */
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

$dispnum = 'ringgroups'; //used for switch on config.php

isset($_REQUEST['action'])?$action = $_REQUEST['action']:$action='';
//the extension we are currently displaying
isset($_REQUEST['extdisplay'])?$extdisplay=$_REQUEST['extdisplay']:$extdisplay='';
isset($_REQUEST['account'])?$account = $_REQUEST['account']:$account='';
isset($_REQUEST['grptime'])?$grptime = $_REQUEST['grptime']:$grptime='';
isset($_REQUEST['grppre'])?$grppre = $_REQUEST['grppre']:$grppre='';
isset($_REQUEST['strategy'])?$strategy = $_REQUEST['strategy']:$strategy='';
isset($_REQUEST['annmsg'])?$annmsg = $_REQUEST['annmsg']:$annmsg='';
isset($_REQUEST['description'])?$description = $_REQUEST['description']:$description='';
isset($_REQUEST['alertinfo'])?$alertinfo = $_REQUEST['alertinfo']:$alertinfo='';
isset($_REQUEST['needsconf'])?$needsconf = $_REQUEST['needsconf']:$needsconf='';
isset($_REQUEST['remotealert'])?$remotealert = $_REQUEST['remotealert']:$remotealert='';
isset($_REQUEST['toolate'])?$toolate = $_REQUEST['toolate']:$toolate='';
isset($_REQUEST['ringing'])?$ringing = $_REQUEST['ringing']:$ringing='';

if (isset($_REQUEST['goto0']) && isset($_REQUEST[$_REQUEST['goto0']."0"])) {
        $goto = $_REQUEST[$_REQUEST['goto0']."0"];
} else {
        $goto = '';
}


if (isset($_REQUEST["grplist"])) {
	$grplist = explode("\n",$_REQUEST["grplist"]);

	if (!$grplist) {
		$grplist = null;
	}
	
	foreach (array_keys($grplist) as $key) {
		//trim it
		$grplist[$key] = trim($grplist[$key]);
		
		// remove invalid chars
		$grplist[$key] = preg_replace("/[^0-9#*]/", "", $grplist[$key]);
		
		if ($grplist[$key] == ltrim($extdisplay,'GRP-').'#')
			$grplist[$key] = rtrim($grplist[$key],'#');
		
		// remove blanks
		if ($grplist[$key] == "") unset($grplist[$key]);
	}
	
	// check for duplicates, and re-sequence
	$grplist = array_values(array_unique($grplist));
}

// do if we are submitting a form
if(isset($_POST['action'])){
	//check if the extension is within range for this user
	if (isset($account) && !checkRange($account)){
		echo "<script>javascript:alert('". _("Warning! Extension")." ".$account." "._("is not allowed for your account").".');</script>";
	} else {
		//add group
		if ($action == 'addGRP') {
			if (ringgroups_add($account,$strategy,$grptime,implode("-",$grplist),$goto,$description,$grppre,$annmsg,$alertinfo,$needsconf,$remotealert,$toolate,$ringing)) {
				needreload();
				redirect_standard('extdisplay');
			}
		}
		
		//del group
		if ($action == 'delGRP') {
			ringgroups_del($account);
			needreload();
			redirect_standard();
		}
		
		//edit group - just delete and then re-add the extension
		if ($action == 'edtGRP') {
			ringgroups_del($account);	
			ringgroups_add($account,$strategy,$grptime,implode("-",$grplist),$goto,$description,$grppre,$annmsg,$alertinfo,$needsconf,$remotealert,$toolate,$ringing);
			needreload();
			redirect_standard('extdisplay');
		}
	}
}
?>
</div>

<div class="rnav"><ul>
    <li><a id="<?php  echo ($extdisplay=='' ? 'current':'') ?>" href="config.php?display=<?php echo urlencode($dispnum)?>"><?php echo _("Add Ring Group")?></a></li>
<?php 
//get unique ring groups
$gresults = ringgroups_list();

if (isset($gresults)) {
	foreach ($gresults as $gresult) {
		echo "<li><a id=\"".($extdisplay=='GRP-'.$gresult[0] ? 'current':'')."\" href=\"config.php?display=".urlencode($dispnum)."&extdisplay=".urlencode("GRP-".$gresult[0])."\">".$gresult[1]." ({$gresult[0]})</a></li>";
	}
}
?>
</ul></div>

<div class="content">
<?php 
if ($action == 'delGRP') {
	echo '<br><h3>'._("Ring Group").' '.$account.' '._("deleted").'!</h3><br><br><br><br><br><br><br><br>';
} else {
	if ($extdisplay) {
		// We need to populate grplist with the existing extension list.
		$thisgrp = ringgroups_get(ltrim($extdisplay,'GRP-'));
		$grpliststr = $thisgrp['grplist'];
		$grplist = explode("-", $grpliststr);
		$strategy = $thisgrp['strategy'];
		$grppre = $thisgrp['grppre'];
		$grptime = $thisgrp['grptime'];
		$goto = $thisgrp['postdest'];
		$annmsg = $thisgrp['annmsg'];
		$description = $thisgrp['description'];
		$alertinfo = $thisgrp['alertinfo'];
		$remotealert = $thisgrp['remotealert'];
		$needsconf = $thisgrp['needsconf'];
		$toolate = $thisgrp['toolate'];
		$ringing = $thisgrp['ringing'];
		unset($grpliststr);
		unset($thisgrp);
		
		$delButton = "
			<form name=delete action=\"{$_SERVER['PHP_SELF']}\" method=POST>
				<input type=\"hidden\" name=\"display\" value=\"{$dispnum}\">
				<input type=\"hidden\" name=\"account\" value=\"".ltrim($extdisplay,'GRP-')."\">
				<input type=\"hidden\" name=\"action\" value=\"delGRP\">
				<input type=submit value=\""._("Delete Group")."\">
			</form>";
			
		echo "<h2>"._("Ring Group").": ".ltrim($extdisplay,'GRP-')."</h2>";
		echo "<p>".$delButton."</p>";
	} else {
		$grplist = explode("-", '');;
		$strategy = '';
		$grppre = '';
		$grptime = '';
		$goto = '';
		$annmsg = '';
		$alertinfo = '';
		$remotealert = '';
		$needsconf = '';
		$toolate = '';
		$ringing = '';

		echo "<h2>"._("Add Ring Group")."</h2>";
	}
	?>
			<form name="editGRP" action="<?php  $_SERVER['PHP_SELF'] ?>" method="post" onsubmit="return checkGRP(editGRP);">
			<input type="hidden" name="display" value="<?php echo $dispnum?>">
			<input type="hidden" name="action" value="<?php echo ($extdisplay ? 'edtGRP' : 'addGRP'); ?>">
			<table>
			<tr><td colspan="2"><h5><?php  echo ($extdisplay ? _("Edit Ring Group") : _("Add Ring Group")) ?><hr></h5></td></tr>
			<tr>
<?php
	if ($extdisplay) { 

?>
				<input size="5" type="hidden" name="account" value="<?php  echo ltrim($extdisplay,'GRP-'); ?>">
<?php 		} else { ?>
				<td><a href="#" class="info"><?php echo _("Ring-Group Number")?>:<span><?php echo _("The number users will dial to ring extensions in this ring group")?></span></a></td>
				<td><input size="5" type="text" name="account" value="<?php  if ($gresult[0]==0) { echo "600"; } else { echo $gresult[0] + 1; } ?>"></td>
<?php 		} ?>
			</tr>
			<tr>
		    <td> <a href="#" class="info"><?php echo _("Group Description:")?>:<span><?php echo _("Provide a descriptive title for this Ring Group.")?></span></a></td>
				<td><input size="20" maxlength="35" type="text" name="description" value="<?php echo htmlspecialchars($description); ?>"></td>
			</tr>
			<tr>
				<td> <a href="#" class="info"><?php echo _("Ring strategy:")?>
				<span>
					<b><?php echo _("ringall")?></b>:  <?php echo _("Ring all available channels until one answers (default)")?><br>
					<b><?php echo _("hunt")?></b>: <?php echo _("Take turns ringing each available extension")?><br>
					<b><?php echo _("memoryhunt")?></b>: <?php echo _("Ring first extension in the list, then ring the 1st and 2nd extension, then ring 1st 2nd and 3rd extension in the list.... etc.")?><br>
                                        <b><?php echo _("*-prim")?></b>:  <?php echo _("These modes act as described above. However, if the primary extension (first in list) is occupied, the other extensions will not be rung. If the primary is freePBX DND, it won't be rung. If the primary is freePBX CF unconditional, then all will be rung")?><br>
				</span>
				</a></td>
				<td>
					<select name="strategy"/>
					<?php
						$default = (isset($strategy) ? $strategy : 'ringall');
                                                $items = array('ringall','ringall-prim','hunt','hunt-prim','memoryhunt','memoryhunt-prim');
						foreach ($items as $item) {
							echo '<option value="'.$item.'" '.($default == $item ? 'SELECTED' : '').'>'._($item);
						}
					?>		
					</select>
				</td>
			</tr>
			<tr>
				<td valign="top"><a href="#" class="info"><?php echo _("Extension list")?>:<span><br><?php echo _("List extensions to ring, one per line.<br><br>You can include an extension on a remote system, or an external number by suffixing a number with a pound (#).  ex:  2448089# would dial 2448089 on the appropriate trunk (see Outbound Routing)<br><br>Extensions will ring the actual extension. If you want them to be sent down channel local (to ring follow-me, other ringgroups, queues, etc. then put a pound (#) at the end. Be careful!")?><br><br></span></a></td>
				<td valign="top">
<?php
		$rows = count($grplist)+1; 
		($rows < 5) ? 5 : (($rows > 20) ? 20 : $rows);
?>
					<textarea id="grplist" cols="15" rows="<?php  echo $rows ?>" name="grplist"><?php echo implode("\n",$grplist);?></textarea><br>
					
					<input type="submit" style="font-size:10px;" value="<?php echo _("Clean & Remove duplicates")?>" />
				</td>
			</tr>
			<tr>
				<td><a href="#" class="info"><?php echo _("CID name prefix")?>:<span><?php echo _('You can optionally prefix the Caller ID name when ringing extensions in this group. ie: If you prefix with "Sales:", a call from John Doe would display as "Sales:John Doe" on the extensions that ring.')?></span></a></td>
				<td><input size="4" type="text" name="grppre" value="<?php  echo $grppre ?>"></td>
			</tr>


			<tr>
				<td><?php echo _("Ring time (max 60 sec)")?>:</td>
				<td><input size="4" type="text" name="grptime" value="<?php  echo $grptime?$grptime:20 ?>"></td>
			</tr>
<?php if(function_exists('recordings_list')) { //only include if recordings is enabled?>
	<tr>
		<td><a href="#" class="info"><?php echo _("Announcement:")?><span><?php echo _("Message to be played to the caller before dialing this group.<br><br>To add additional recordings please use the \"System Recordings\" MENU to the left")?></span></a></td>
		<td>
			<select name="annmsg"/>
			<?php
				$tresults = recordings_list();
				$default = (isset($annmsg) ? $annmsg : '');
				echo '<option value="">'._("None")."</option>";
				if (isset($tresults[0])) {
					foreach ($tresults as $tresult) {
						echo '<option value="'.$tresult[2].'"'.($tresult[2] == $default ? ' SELECTED' : '').'>'.$tresult[1]."</option>\n";
					}
				}
			?>		
			</select>		
		</td>
	</tr>
<?php }	else { ?>
	<tr>
		<td><a href="#" class="info"><?php echo _("Announcement:")?><span><?php echo _("Message to be played to the caller before dialing this group.<br><br>You must install and enable the \"Systems Recordings\" Module to edit this option")?></span></a></td>
		<td>
			<?php
				$default = (isset($annmsg) ? $annmsg : '');
			?>
			<input type="hidden" name="annmsg" value="<?php echo $default; ?>"><?php echo ($default != '' ? $default : 'None'); ?>
		</td>
	</tr>
<?php } if (function_exists('music_list')) { ?>
	<tr>
		<td><a href="#" class="info"><?php echo _("Play Music On Hold?")?><span><?php echo _("If you select a Music on Hold class to play, instead of 'Ring', they will hear that instead of Ringing while they are waiting for someone to pick up.")?></span></a></td>
		<td>
			<select name="ringing"/>
			<?php
				$tresults = music_list("/var/lib/asterisk/mohmp3");
				$cur = (isset($ringing) ? $ringing : 'Ring');
				echo '<option value="Ring">'._("Ring")."</option>";
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
		<td><a href="#" class="info"><?php echo _("Alert Info")?><span><?php echo _('ALERT_INFO can be used for distinctive ring with SIP devices.')?></span></a>:</td>
		<td><input type="text" name="alertinfo" size="10" value="<?php echo ($alertinfo)?$alertinfo:'' ?>"></td>
	</tr>
	<tr>
		<td><a href="#" class="info"><?php echo _("Confirm Calls")?><span><?php echo _('Enable this if you\'re calling external numbers that need confirmation - eg, a mobile phone may go to voicemail which will pick up the call. Enabling this requires the remote side push 1 on their phone before the call is put through. This feature only works with the ringall ring strategy')?></span></a>:</td>
		<td> <?php if (!function_exists('recordings_list')) { echo _("System Recordings not installed. Option Disabled"); } else { ?>
			<input type="checkbox" name="needsconf" value="CHECKED" <?php echo $needsconf ?>  /></td>
<?php } ?>
	</tr>
<?php if(function_exists('recordings_list')) { //only include if recordings is enabled?>
	<tr>
		<td><a href="#" class="info"><?php echo _("Remote Announce:")?><span><?php echo _("Message to be played to the person RECEIVING the call, if 'Confirm Calls' is enabled.<br><br>To add additional recordings use the \"System Recordings\" MENU to the left")?></span></a></td>
		<td>
			<select name="remotealert"/>
			<?php
				$tresults = recordings_list();
				$default = (isset($remotealert) ? $remotealert : '');
				echo '<option value="">'._("Default")."</option>";
				if (isset($tresults[0])) {
					foreach ($tresults as $tresult) {
						echo '<option value="'.$tresult[2].'"'.($tresult[2] == $default ? ' SELECTED' : '').'>'.$tresult[1]."</option>\n";
					}
				}
			?>		
			</select>		
		</td>
	</tr>
	<tr>
		<td><a href="#" class="info"><?php echo _("Too-Late Announce:")?><span><?php echo _("Message to be played to the person RECEIVING the call, if the call has already been accepted before they push 1.<br><br>To add additional recordings use the \"System Recordings\" MENU to the left")?></span></a></td>
		<td>
			<select name="toolate"/>
			<?php
				$tresults = recordings_list();
				$default = (isset($toolate) ? $toolate : '');
				echo '<option value="">'._("Default")."</option>";
				if (isset($tresults[0])) {
					foreach ($tresults as $tresult) {
						echo '<option value="'.$tresult[2].'"'.($tresult[2] == $default ? ' SELECTED' : '').'>'.$tresult[1]."</option>\n";
					}
				}
			?>		
			</select>		
		</td>
	</tr>
<?php } ?>
			<tr><td colspan="2"><br><h5><?php echo _("Destination if no answer")?>:<hr></h5></td></tr>

<?php 
//draw goto selects
echo drawselects($goto,0);
?>
			
			<tr>
			<td colspan="2"><br><h6><input name="Submit" type="submit" value="<?php echo _("Submit Changes")?>"></h6></td>		
			
			</tr>
			</table>
			</form>
<?php 		
		} //end if action == delGRP
		

?>
<script language="javascript">
<!--

function checkGRP(theForm) {
	var msgInvalidGrpNum = "<?php echo _('Invalid Group Number specified'); ?>";
	var msgInvalidGrpNumStartWithZero = "<?php echo _('Group numbers with more than one digit cannot begin with 0'); ?>";
	var msgInvalidExtList = "<?php echo _('Please enter an extension list.'); ?>";
	var msgInvalidGrpPrefix = "<?php echo _('Invalid Caller ID prefix.'); ?>";
	var msgInvalidTime = "<?php echo _('Invalid time specified'); ?>";
	var msgInvalidGrpTimeRange = "<?php echo _('Time must be between 1 and 60 seconds'); ?>";
	var msgInvalidDescription = "<?php echo _('Please enter a valid Group Description'); ?>";
	var msgInvalidRingStrategy = "<?php echo _('You must choose ringall or ringall-prim ring strategy when using Confirm Calls'); ?>";

	// set up the Destination stuff
	setDestinations(theForm, 1);

	// form validation
	defaultEmptyOK = false;
	if (!isInteger(theForm.account.value)) {
		return warnInvalid(theForm.account, msgInvalidGrpNum);
	} else if (theForm.account.value.indexOf('0') == 0 && theForm.account.value.length > 1) {
		return warnInvalid(theForm.account, msgInvalidGrpNumStartWithZero);
	}
	
	defaultEmptyOK = false;	
	if (!isAlphanumeric(theForm.description.value))
		return warnInvalid(theForm.description, msgInvalidDescription);
	
	if (isEmpty(theForm.grplist.value))
		return warnInvalid(theForm.grplist, msgInvalidExtList);

	defaultEmptyOK = true;
	if (!isCallerID(theForm.grppre.value))
		return warnInvalid(theForm.grppre, msgInvalidGrpPrefix);
	
	defaultEmptyOK = false;
	if (!isInteger(theForm.grptime.value)) {
		return warnInvalid(theForm.grptime, msgInvalidTime);
	} else {
		var grptimeVal = theForm.grptime.value;
		if (grptimeVal < 1 || grptimeVal > 60)
			return warnInvalid(theForm.grptime, msgInvalidGrpTimeRange);
	}

	if (theForm.needsconf.checked && theForm.strategy.value.substring(0,7) != "ringall") {
		return warnInvalid(theForm.needsconf, msgInvalidRingStrategy);
	}

	if (!validateDestinations(theForm, 1, true))
		return false;
		
	return true;
}
//-->
</script>

