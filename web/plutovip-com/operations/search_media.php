<?php
 function search_media($output) {
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
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=use_mobile_phone"><img src="images/submenus/How%20Pluto%20works/mobile_phone_off.gif" width="230" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=search_media"><img src="images/submenus/How%20Pluto%20works/search_media_on.gif" width="230" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=dvd_remote"><img src="images/submenus/How%20Pluto%20works/dvd_remote_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=tv_remote"><img src="images/submenus/How%20Pluto%20works/tv_remote_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=manage_house"><img src="images/submenus/How%20Pluto%20works/manage_your_house_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=handle_security"><img src="images/submenus/How%20Pluto%20works/handle_security_event_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=manage_call"><img src="images/submenus/How%20Pluto%20works/manage_call_progress_off.gif" width="230" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
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
    <p><img src="images/submenus/search_media_txt.gif" width="183" height="13"><br>
        <br>
      No matter what media you're looking for, just press 'search' from any of the media screens and start typing.  Pluto will automatically show you everything that's relevant--actors, movies, song titles, groups.  You don't even need to think about it.<p></p>Here we typed in 'beat', and if we touch any item in the results, we'll see the details, and can press 'play' to play it now, or 'play Queue' to add it to our queue.  Once you've added media to your queue, you can save it as a playlist.</p>
      <p>In this case, we'll touch 'Beatles' (the group), and 'view'. That takes us to the lower right screen, which shows, you all the Beatle's songs and albums.</p>
          
      <p><center><a href="javascript:open_popup('operations/poza8.html','532','465')"><img src="images/img%20how%20to/03/03_music_m.jpg" width="141" height="112" border="0"></a> <a href="javascript:open_popup('operations/poza9.html','532','465')"><img src="images/img%20how%20to/03/03_music_2_m.jpg" width="141" height="112" border="0"></a>
      </center></p></td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(2); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::How to Search for Media");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>