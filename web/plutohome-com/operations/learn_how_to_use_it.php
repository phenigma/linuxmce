<?
function learn_how_to_use_it($output,$conn){

	$imgArray=array();
	$phoneThumbsArray=array('p-main-menu.jpg'=>'Main Menu','p-lights-menu.jpg'=>'Lights Menu','p-media-menu.jpg'=>'Media Menu','p-telecom.jpg'=>'Telecom Menu','p-climate.jpg'=>'Climate Menu','p-music-remote.jpg'=>'Music remote','p-tv.jpg'=>'TV remote','p-dvd-remote.jpg'=>'DVD remote','p-quad-view.jpg'=>'Quad view');
	$pos=0;
	foreach ($phoneThumbsArray as $thumb=>$text){
		if($pos==5){
			$imgArray[]='</tr><table><table align="center"><tr>';
		}
		$imgArray[]='<td align="center" valign="top"><a href="javascript:open_popup(\'operations/image.php?img=screenshots/'.$thumb.'&single=1\',\'width=420,height=480,scrollbars=0,toolbar=0,resizable=0\');"><img src="images/screenshots/thumbs/'.$thumb.'" border="1" alt="Click to see larger image" style="border: 1px outset #B9D1FD;"></a><br>'.$text.'</td>';
		$pos++;
	}
	$phoneScreenshots='<table border="0" align="center"><tr>'.join('',$imgArray).'</tr></table>';

	$tabletArray=array();
	$tabletThumbsArray=array('t-main-menu.jpg'=>'Main menu','t-media-map.jpg'=>'Media map','t-music-remote.jpg'=>'Music remote','t-save-playlist.jpg'=>'Save playlist','t-tv.jpg'=>'TV remote','t-dvd-remote.jpg'=>'DVD remote','t-quad-view.jpg'=>'Quad view');
	$pos=0;
	foreach ($tabletThumbsArray as $thumb=>$text){
		if($pos==4){
			$tabletArray[]='</tr><table><table align="center"><tr>';
		}
		$tabletArray[]='<td align="center"><a href="javascript:open_popup(\'operations/image.php?img=screenshots/'.$thumb.'&single=1\',\'width=820,height=620,scrollbars=0,toolbar=0,resizable=0\');"><img src="images/screenshots/thumbs/'.$thumb.'" border="1" alt="Click to see larger image" style="border: 1px outset #B9D1FD;"></a><br>'.$text.'</td>';
		$pos++;
	}
	$tabletScreenshots='<table border="0" align="center"><tr>'.join(' ',$tabletArray).'</tr></table>';
	
	$out = <<<TEXT
	
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="left" class="insidetable"><table width="438"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="155"><a href="index.php?section=pluto_system_overview"><img src="images/buttons/pluto_system_overview_off.gif" width="155" height="11" border="0" /></a></td>
        <td width="155"><a href="index.php?section=how_the_system_works"><img src="images/buttons/how_the_system_works_off.gif" width="155" height="11" border="0" /></a></td>
        <td width="128"><a href="index.php?section=learn_how_to_use_it"><img src="images/buttons/learn_how_to_use_it_on.gif" width="128" height="11" border="0" /></a></td>
        </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_how_to_use_it.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/learn_how_to_use_it.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
                  <a href="index.php?section=learn_how_to_use_it#mobile">How to use the mobile phone</a><br />
                  <a href="index.php?section=learn_how_to_use_it#orbiter">How to use the orbiter</a><br />
                  <a href="index.php?section=learn_how_to_use_it#follow">How pluto's follow me works</a><br /></td>
            </tr>
            <tr>
              <td align="right">&nbsp; </td>
            </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2"><a name="mobile"><img src="images/bullet_1.gif" width="11" height="11" border="0" align="absmiddle" /></a> <span class="title">How to use the mobile phone </span><br />
      When you're home, the lower left part of the screen always shows what room you are in, and the menu shows whatever activities apply to that room. The buttons 1-5 are each of your main categories. So, if you press '1-lights' it lists all the lighting and mood scenes for that room, including drapes &amp; blinds. '2-media' lists all your media choices, '3-climate' climate options, including pool, jacuzzi and sprinklers. '4-telecom' sets your status and shows voice messages. '5-security' sets the alarm mode and also shows surveillance cameras directly on the phone, as well as lock/unlock any doors and broadcast your voice through parts of the house.
      $phoneScreenshots
      <p><a name="orbiter"><img src="images/bullet_1.gif" width="11" height="11" border="0" align="absmiddle" /></a> <span class="title">How to use the orbiter</span><br />
        Because the Orbiters are shared by the whole family, unlike the mobile phone, you need to tell it who is using it, so it can customize itself to your taste. If you're not that person, just touch the picture, and you'll see pictures of all the family members. So, if you want to control a room other than the one shown, just touch the button to select another room. The maps are &quot;lights&quot;, &quot;media&quot;, &quot;climate&quot;, &quot;phone&quot;, and &quot;security&quot;. <br />
        The 'lighting' buttons are all your lighting scenarios or moods. A mood can do more than just control lights. For example, 'Bedtime' could play some background music and set your status to 'do not disturb'. The 'media' buttons are all the media choices in this room. The 'climate' buttons are your scenarios which control the climate equipments and 'environment', like pool and sprinkler. Touch the 'Phone' button to make a phone call. You will see your personal phone book and speed dials right on the tablet for easy browsing and when you touch someone to call, Pluto will make the call for you and put it through on the nearest phone. The other 'telecom' buttons are the current status of each family member, including the voice messages. The 'blue' security button lets you set the alarm mode and the various camera buttons are shortcuts to view surveillance cameras on the tablet - either quad view or single view. You can add miscellaneous options for each room.</p>
      $tabletScreenshots
      <p><a name="follow"><img src="images/bullet_1.gif" width="11" height="11" border="0" align="absmiddle" /></a> <span class="title">How pluto's follow me works</span><br />
    Pluto's engineers set out on the quest of turning the cell phone into a remote control and developing a very inexpensive way to identify the unique ID that each cell phone broadcasts. That technology is now built into every Pluto Media Director. As you wander around the house, each Media Director is constantly reporting to the Pluto Core the signal strength readings from each family member's mobile phone. The Pluto Core collects this information and is able to instantly determine where in the house each mobile phone is. It takes between 5 and 10 seconds for the Core to identify each person after entering the room. If there is a motion detector in the room, Pluto can turn on the lights instantly as soon as movement is detected and then adjust them to your personal lighting preference a few seconds later when it has identified you. There is no practical limit to the number of people Pluto can track as long as each person has his own phone and carries it with him.</p></td>
  </tr>
  <tr>
    <td valign="top">&nbsp;</td>
  </tr>
</table>

TEXT;


	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works','Learn how to use it'=>'index.php?section=learn_how_to_use_it'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>