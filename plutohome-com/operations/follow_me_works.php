<?php
 function follow_me_works($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/how_pluto_works_txt.gif" width="127" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=follow_me_works"><img src="images/submenus/How%20Pluto%20works/follow-me_works_on.gif" width="230" height="17" border="0"></a></td>
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
        <td><a href="index.php?section=handle_security"><img src="images/submenus/How%20Pluto%20works/handle_security_event_off.gif" width="230" height="17" border="0"></a></td>
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
      <p><img src="images/submenus/follow-me_works_txt.gif" width="228" height="13"></p>
      <p>It has been widely publicized that a handful of billionaires have smart houses that are able to recognize each family member so his media and other preferences can follow him around the house.  The first problem with these systems is that they cost millions of dollars.  But there was another drawback: the homeowner had to always carry around a special device that the system would recognize.  Great--something else to lug around!</p>
      <p>Pluto took a fresh look at the problem and recognized that today, many people already carry their cell phones at all times. If we could also turn the cell phone into the remote control for everything in the house, even those people who didn't already carry it around would now be inclined to do so. Cell phones make perfect remote controls. They have nice, color displays. They're small, light, comfortable, and can be operated with only the thumb. You can't get more convenient than having a small cell phone in your pocket that let's you press one button to get the lights, or the tv, or the a/c. It's a lot easier than running to a switch, or turning over couch cushions to look for the remote, or carrying around a big home automation tablet.</p>
      <p><center><a href="javascript:open_popup('operations/poza1.html','532','465')"><img src="images/img%20how%20to/01/01_mainmenu_m.jpg" width="141" height="112" hspace="5" vspace="5" border="0"></a></center></p>
      <p>This presented the ideal solution: we now have the world's perfect remote control, and as such, a device that every homeowner will want to carry around all the time. So Pluto's engineers set out on the quest of turning the cell into a remote control, and developing a very inexpensive way to identify the unique ID that each cell phone broadcasts. After much work and many patent applications, that technology is now built into every Pluto Media Director. As you wander around the house, each Media Director is constantly reporting to the Pluto Core the signal strength readings from each family member's mobile phone. The Pluto Core collects this information and is able to instantly determine where in the house each mobile phone is. It takes between 5 and 10 seconds for the Core to identify each person after entering the room. If there is a motion detector in the room, Pluto can turn on the lights instantly as soon as movement is detected, and then adjust them to your personal lighting preference a few seconds later when it has identified you. There is no practical limit to the number of people Pluto can track as long as each person has his own phone and carries it with him.</p>
    </td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(2); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::How Pluto's follow-me works");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>