<?php
 function handle_security($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/how_pluto_works_txt.gif" width="127" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=follow_me_works"><img src="images/submenus/How%20Pluto%20works/follow-me_works_off.gif" width="230" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=use_mobile_phone"><img src="images/submenus/How%20Pluto%20works/mobile_phone_off.gif" width="230" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=search_media"><img src="images/submenus/How%20Pluto%20works/search_media_off.gif" width="230" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=dvd_remote"><img src="images/submenus/How%20Pluto%20works/dvd_remote_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=tv_remote"><img src="images/submenus/How%20Pluto%20works/tv_remote_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=manage_house"><img src="images/submenus/How%20Pluto%20works/manage_your_house_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=handle_security"><img src="images/submenus/How%20Pluto%20works/handle_security_event_on.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=manage_call"><img src="images/submenus/How%20Pluto%20works/manage_call_progress_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=maps"><img src="images/submenus/How%20Pluto%20works/maps_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
    <p><img src="images/submenus/handle_security_event_txt.gif" width="231" height="13"></p>
      <p>Whenever someone rings the door or trips the alarm, the Pluto Core instantly begins capturing surveillance video from whatever cameras are in that area.  If your mobile phone is turned on and you have coverage, your phone will alert you and show you a still frame taken the instant the sensor or doorbell was tripped, so you can see exactly what caused the event.  You can press a button on the phone to start viewing this, or any of your surveillance cameras live.  If there is a door associated with that camera, the * button will unlock the door.  The green 'talk' button will turn on the tv's and stereo's and cause Pluto to join them in a big conference call, putting you through on the mobile phone.  If there is an intercom, you can speak to the person through it instead.  This allows you to converse with the person.  Of course, you have the usual menu options and can turn on and off lights and control everything else in your house too.  If this is an emergency, press the '911' button and Pluto will make an emergency call from the home phone (so the police see a real emergency with your address) and then patch you in on your mobile phone. </p>
      <p><center><a href="javascript:open_popup('operations/poza17.html','532','465')"><img src="images/img%20how%20to/07/07_alert_m.jpg" width="141" height="112" border="0"></a>
      </center></p></td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(2); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::How to Handle a Security Event");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>